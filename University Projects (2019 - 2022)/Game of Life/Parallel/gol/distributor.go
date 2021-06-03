package gol

import (
	"fmt"
	"os"
	"strconv"
	"sync"
	"time"

	"uk.ac.bris.cs/gameoflife/util"
)

const alive = 255
const dead = 0

const noOperation = -1
const savePgm = 0
const pause = 1
const unpause = 2
const terminate = 3

type matrixSlice struct {
	matrix          [][]uint8
	startY, finishY int
}

type distributorChannels struct {
	events        chan<- Event
	ioCommand     chan<- ioCommand
	ioIdle        <-chan bool
	fileChannel   chan<- string
	inputChannel  <-chan uint8
	outputChannel chan<- uint8
	keyPresses    <-chan rune
}

func mod(x, m int) int {
	return (x + m) % m
}

func sendCellFlipped(events chan<- Event, turn int, cell util.Cell) {
	events <- CellFlipped{CompletedTurns: turn, Cell: cell}
}

func sendTurnComplete(events chan<- Event, turn int) {
	events <- TurnComplete{CompletedTurns: turn}
}

func sendFinalTurnComplete(events chan<- Event, turn int, cells []util.Cell) {
	events <- FinalTurnComplete{CompletedTurns: turn, Alive: cells}
}

func sendImageOutputComplete(events chan<- Event, turn int, completeOutputPath string) {
	events <- ImageOutputComplete{CompletedTurns: turn, Filename: completeOutputPath}
}

func sendAliveCellsCount(events chan<- Event, turn, aliveCount int) {
	events <- AliveCellsCount{CompletedTurns: turn, CellsCount: aliveCount}
}

func calculateNeighbours(p Params, x, y int, world [][]uint8) int {
	neighbours := 0
	for i := -1; i <= 1; i++ {
		for j := -1; j <= 1; j++ {
			if i != 0 || j != 0 {
				if world[mod(y+i, p.ImageHeight)][mod(x+j, p.ImageWidth)] == alive {
					neighbours++
				}
			}
		}
	}
	return neighbours
}

func nextIteration(world [][]uint8, p Params, c distributorChannels, turn, startY, finishY int, out chan<- matrixSlice) {
	var newWorld [][]uint8
	newWorld = make([][]byte, finishY-startY)
	for i := range newWorld {
		newWorld[i] = make([]byte, p.ImageWidth)
	}
	for y := startY; y < finishY; y++ {
		for x := 0; x < p.ImageWidth; x++ {
			newWorld[y-startY][x] = world[y][x]
			currentNeighbours := calculateNeighbours(p, x, y, world)

			if world[y][x] == alive {
				if currentNeighbours < 2 || currentNeighbours > 3 {
					newWorld[y-startY][x] = dead
					cell := util.Cell{Y: y, X: x}
					sendCellFlipped(c.events, turn, cell)
				}
			} else {
				if currentNeighbours == 3 {
					newWorld[y-startY][x] = alive
					cell := util.Cell{Y: y, X: x}
					sendCellFlipped(c.events, turn, cell)
				}
			}
		}
	}
	out <- matrixSlice{matrix: newWorld, startY: startY, finishY: finishY}
}

func makeInitialTurn(aliveCells []util.Cell, c distributorChannels) {
	for _, cell := range aliveCells {
		cellFlippedCurrent := CellFlipped{CompletedTurns: 0, Cell: cell}
		c.events <- cellFlippedCurrent
	}
	sendTurnComplete(c.events, 0)
}

func createWorld(height, width int, inputChannel <-chan uint8) [][]uint8 {
	world := make([][]byte, height)
	for i := range world {
		world[i] = make([]byte, width)
	}

	for i := 0; i < height; i++ {
		for j := 0; j < width; j++ {
			world[i][j] = <-inputChannel
		}
	}

	return world
}

func activateReadPgm(p Params, c distributorChannels, completeInputPath string) {
	c.ioCommand <- 1
	c.fileChannel <- completeInputPath
}

func activateWritePgm(p Params, c distributorChannels, smallOutputPath string, cells []util.Cell) {
	c.ioCommand <- 0
	c.fileChannel <- smallOutputPath

	finalWorld := make([][]byte, p.ImageHeight)
	for i := range finalWorld {
		finalWorld[i] = make([]byte, p.ImageWidth)
	}

	for _, cell := range cells {
		finalWorld[cell.Y][cell.X] = alive
	}

	for i := 0; i < p.ImageHeight; i++ {
		for j := 0; j < p.ImageWidth; j++ {
			c.outputChannel <- finalWorld[i][j]
		}
	}

	_ = <-c.ioIdle
}

func createIoChannels(commandChannel <-chan ioCommand, idleChannel chan<- bool, fileChannel <-chan string, outputChannel <-chan uint8, inputChannel chan<- uint8) ioChannels {
	return ioChannels{
		command:  commandChannel,
		idle:     idleChannel,
		filename: fileChannel,
		output:   outputChannel,
		input:    inputChannel,
	}
}

func executeThreads(p Params, separateThread, turn int, world, extraWorld [][]uint8, c distributorChannels, matrixChannel chan matrixSlice, mutex *sync.Mutex) {
	startY := 0
	incrementY := p.ImageHeight / p.Threads
	var aux matrixSlice

	for i := 1; i <= p.Threads-separateThread; i++ {
		go nextIteration(world, p, c, turn, startY, startY+incrementY, matrixChannel)
		startY = startY + incrementY
	}

	if separateThread == 1 {
		go nextIteration(world, p, c, turn, startY, p.ImageHeight, matrixChannel)
	}

	for i := 1; i <= p.Threads; i++ {
		aux = <-matrixChannel

		for j := aux.startY; j < aux.finishY; j++ {
			for k := 0; k < p.ImageWidth; k++ {
				extraWorld[j][k] = aux.matrix[j-aux.startY][k]
			}
		}
	}

	mutex.Lock()
	for i := range world {
		copy(world[i], extraWorld[i])
	}
	mutex.Unlock()
}

func checkOperation(p Params, world [][]uint8, c distributorChannels, writeDone chan<- bool, operationType *int, savePgm, turn int) {
	if *operationType == savePgm {
		*operationType = noOperation
		currentlyAlive := getAliveCells(world, p.ImageHeight, p.ImageWidth)
		activateWritePgm(p, c, "turn"+strconv.Itoa(turn), currentlyAlive)
		writeDone <- true
	} else if *operationType == pause {
		for *operationType != unpause {
			fmt.Println("waiting")
		}
		fmt.Println("Continuing")
		*operationType = noOperation
	} else if *operationType == terminate {
		os.Exit(0)
	}
}

func trackTicker(ticker *time.Ticker, turn *int, p Params, world *[][]uint8, c distributorChannels, tickerDone chan bool, mutex *sync.Mutex) {
	for {
		select {
		case _ = <-ticker.C:
			mutex.Lock()
			currentlyAlive := getAliveCells(*world, p.ImageHeight, p.ImageWidth)
			sendAliveCellsCount(c.events, *turn, len(currentlyAlive))
			mutex.Unlock()
		case _ = <-tickerDone:
			ticker.Stop()
			return
		}
	}
}

func executeTurns(p Params, c distributorChannels, world [][]uint8, turn int, writeDone chan<- bool, operationType *int) {
	extraWorld := make([][]uint8, len(world))
	for i := range world {
		extraWorld[i] = make([]uint8, len(world[i]))
		copy(extraWorld[i], world[i])
	}

	separateThread := 0
	tickerDone := make(chan bool)
	matrixChannel := make(chan matrixSlice, p.Threads)

	if p.ImageHeight%p.Threads != 0 {
		separateThread = 1
	}

	var mutex = &sync.Mutex{}
	ticker := time.NewTicker(2 * time.Second)
	go trackTicker(ticker, &turn, p, &world, c, tickerDone, mutex)

	for turn != p.Turns {
		executeThreads(p, separateThread, turn, world, extraWorld, c, matrixChannel, mutex)
		turn++
		checkOperation(p, world, c, writeDone, operationType, savePgm, turn)
		sendTurnComplete(c.events, turn)
	}
	tickerDone <- true
}

func getAliveCells(world [][]uint8, height, width int) []util.Cell {
	var cells []util.Cell
	for y := 0; y < height; y++ {
		for x := 0; x < width; x++ {
			if world[y][x] != 0 {
				cells = append(cells, util.Cell{
					X: x,
					Y: y,
				})
			}
		}
	}
	return cells
}

func manageSdl(operationType *int, writeDone <-chan bool, keyPresses <-chan rune) {
	for {
		aux := <-keyPresses
		if aux == 's' && *operationType != pause {
			*operationType = savePgm
			_ = <-writeDone
		} else if aux == 'p' {
			if *operationType == noOperation {
				*operationType = pause
			} else if *operationType == pause {
				*operationType = unpause
			}
		} else if aux == 'q' && *operationType != pause {
			*operationType = terminate
		}
		fmt.Println(aux)
	}
}

// distributor divides the work between workers and interacts with other goroutines.
func distributor(p Params, c distributorChannels) {
	// TODO: Create a 2D slice to store the world.
	// TODO: For all initially alive cells send a CellFlipped Event.

	turn := 0
	completeInputPath := fmt.Sprintf("%dx%d", p.ImageWidth, p.ImageHeight)
	smallInputPath := "images/" + completeInputPath + ".pgm"
	completeOutputPath := "out/" + completeInputPath + "x" + strconv.Itoa(p.Turns) + ".pgm"
	smallOutputPath := completeInputPath + "x" + strconv.Itoa(p.Turns)
	aliveCells := util.ReadAliveCells(smallInputPath, p.ImageWidth, p.ImageHeight)

	makeInitialTurn(aliveCells, c)

	go activateReadPgm(p, c, completeInputPath)
	world := createWorld(p.ImageHeight, p.ImageWidth, c.inputChannel)

	// TODO: Execute all turns of the Game of Life.
	// TODO: Send correct Events when required, e.g. CellFlipped, TurnComplete and FinalTurnComplete.
	//		 See event.go for a list of all events.

	operationType := noOperation
	writeDone := make(chan bool)
	go manageSdl(&operationType, writeDone, c.keyPresses)

	executeTurns(p, c, world, turn, writeDone, &operationType)
	cells := getAliveCells(world, p.ImageHeight, p.ImageWidth)
	sendFinalTurnComplete(c.events, turn, cells)
	activateWritePgm(p, c, smallOutputPath, cells)
	sendImageOutputComplete(c.events, turn, completeOutputPath)

	// Make sure that the Io has finished any output before exiting.
	c.ioCommand <- ioCheckIdle
	<-c.ioIdle

	c.events <- StateChange{turn, Quitting}
	// Close the channel to stop the SDL goroutine gracefully. Removing may cause deadlock.
	close(c.events)
}
