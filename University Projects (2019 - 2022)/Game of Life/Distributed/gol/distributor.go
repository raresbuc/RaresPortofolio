package gol

import (
	"fmt"
	"net/rpc"
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
const shutDownAll = 4

type Response struct {
	GolChange [][]uint8
}

type Request struct {
	GolWorld [][]uint8
	Bound    int
}

type ResponseStop struct {
}

type RequestStop struct {
	Message string
}

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

func checkOperation(p Params, world [][]uint8, c distributorChannels, writeDone chan<- bool, operationType *int, savePgm, turn int, client []*rpc.Client) {
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
	} else if *operationType == shutDownAll {
		currentlyAlive := getAliveCells(world, p.ImageHeight, p.ImageWidth)
		activateWritePgm(p, c, "finalTurn", currentlyAlive)
		requestStop := RequestStop{Message: "k"}
		responseStop := new(ResponseStop)
		for i := 0; i < p.Threads; i++ {
			client[i].Call("WorldOperation.StopServer", requestStop, responseStop)
		}
		os.Exit(0)
	}
}

func manageSdl(operationType *int, writeDone <-chan bool, keyPresses <-chan rune, p Params, world [][]uint8, c distributorChannels) {
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
		} else if aux == 'k' && *operationType != pause {
			*operationType = shutDownAll
			_ = <-writeDone
		}
		fmt.Println(aux)
	}
}

func update(golChange [][]uint8, startY, finishY, width, bound, turn int, world [][]uint8, events chan<- Event, doneMatrixSlice chan bool) {
	offsetBound := 0
	if bound == 1 {
		offsetBound = 1
	}

	for i := 0 + offsetBound; i < len(golChange)-offsetBound; i++ {
		for j := 0; j < width; j++ {
			if golChange[i][j] == 1 {
				cell := util.Cell{Y: i + startY - offsetBound, X: j}
				sendCellFlipped(events, turn, cell)
				if world[i+startY-offsetBound][j] == dead {
					world[i+startY-offsetBound][j] = alive
				} else {
					world[i+startY-offsetBound][j] = dead
				}
			}
		}
	}

	doneMatrixSlice <- true
}

func makeWorldSlice(world [][]uint8, startY, finishY int, upperBound []uint8, lowerBound []uint8) [][]uint8 {
	boundCnt := 0
	boundOffset := 0
	if len(upperBound) != 0 {
		boundCnt = 2
		boundOffset = 1
	}

	var worldSlice [][]uint8
	worldSlice = make([][]byte, finishY-startY+boundCnt)
	for i := range worldSlice {
		worldSlice[i] = make([]byte, len(world[0]))
	}

	if boundCnt == 2 {
		copy(worldSlice[0], upperBound)
	}

	for i := startY; i < finishY; i++ {
		copy(worldSlice[i-startY+boundOffset], world[i])
	}

	if boundCnt == 2 {
		copy(worldSlice[len(worldSlice)-1], lowerBound)
	}

	return worldSlice
}

func executeThread(currentThread, incrementY, bound int, p Params, world [][]uint8, request []Request, response []*Response, currentClient *rpc.Client, readyIndex chan<- int) {
	startY := currentThread * incrementY
	var finishY int
	if currentThread == p.Threads-1 {
		finishY = p.ImageHeight
	} else {
		finishY = startY + incrementY
	}
	if bound == 1 {
		upperIndex := mod(startY-1, p.ImageHeight)
		lowerIndex := mod(finishY, p.ImageHeight)
		worldSlice := makeWorldSlice(world, startY, finishY, world[upperIndex], world[lowerIndex])
		request[currentThread] = Request{GolWorld: worldSlice, Bound: bound}
	} else {
		worldSlice := makeWorldSlice(world, startY, finishY, []uint8{}, []uint8{})
		request[currentThread] = Request{GolWorld: worldSlice, Bound: bound}
	}

	response[currentThread] = new(Response)
	currentClient.Call("WorldOperation.GetNextIteration", request[currentThread], response[currentThread])
	readyIndex <- currentThread
}

func readyUpdate(incrementY, currentIndex, bound, turn int, p Params, response []*Response, world [][]uint8, c distributorChannels, doneMatrixSlice chan bool) {
	startY := incrementY * currentIndex
	if currentIndex == p.Threads-1 {
		finishY := p.ImageHeight
		update(response[currentIndex].GolChange, startY, finishY, p.ImageWidth, bound, turn, world, c.events, doneMatrixSlice)
	} else {
		finishY := startY + incrementY
		update(response[currentIndex].GolChange, startY, finishY, p.ImageWidth, bound, turn, world, c.events, doneMatrixSlice)
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

func executeTurns(world [][]uint8, p Params, c distributorChannels, turn int, operationType *int, client []*rpc.Client, writeDone chan<- bool) {
	incrementY := p.ImageHeight / p.Threads
	separateThread := 0
	bound := 0
	if p.Threads != 1 {
		bound = 1
	}
	if p.ImageHeight%p.Threads != 0 {
		separateThread = 1
	}

	readyIndex := make(chan int)
	response := make([]*Response, p.Threads)
	request := make([]Request, p.Threads)
	tickerDone := make(chan bool)

	var mutex = &sync.Mutex{}
	extraWorld := make([][]uint8, len(world))
	for i := range world {
		extraWorld[i] = make([]uint8, len(world[i]))
		copy(extraWorld[i], world[i])
	}

	ticker := time.NewTicker(2 * time.Second)
	go trackTicker(ticker, &turn, p, &world, c, tickerDone, mutex)
	for turn != p.Turns {
		for i := 0; i < p.Threads-separateThread; i++ {
			go executeThread(i, incrementY, bound, p, extraWorld, request, response, client[i], readyIndex)
		}

		if separateThread == 1 {
			currentThread := p.Threads - 1
			go executeThread(currentThread, incrementY, bound, p, extraWorld, request, response, client[currentThread], readyIndex)
		}

		cnt := 0
		doneMatrixSlice := make(chan bool, p.Threads)
		go func() {
			for cnt != p.Threads {
				currentIndex := <-readyIndex
				cnt++
				go readyUpdate(incrementY, currentIndex, bound, turn, p, response, extraWorld, c, doneMatrixSlice)
			}
		}()
		for i := 0; i < p.Threads; i++ {
			_ = <-doneMatrixSlice
		}
		mutex.Lock()
		for i := range world {
			copy(world[i], extraWorld[i])
		}
		mutex.Unlock()
		turn++
		checkOperation(p, world, c, writeDone, operationType, savePgm, turn, client)
		sendTurnComplete(c.events, turn)
	}
	tickerDone <- true
}

// distributor divides the work between workers and interacts with other goroutines.
func distributor(p Params, c distributorChannels) {
	turn := 0
	completeInputPath := fmt.Sprintf("%dx%d", p.ImageWidth, p.ImageHeight)
	smallInputPath := "images/" + completeInputPath + ".pgm"
	completeOutputPath := "out/" + completeInputPath + "x" + strconv.Itoa(p.Turns) + ".pgm"
	smallOutputPath := completeInputPath + "x" + strconv.Itoa(p.Turns)
	aliveCells := util.ReadAliveCells(smallInputPath, p.ImageWidth, p.ImageHeight)

	makeInitialTurn(aliveCells, c)

	go activateReadPgm(p, c, completeInputPath)
	world := createWorld(p.ImageHeight, p.ImageWidth, c.inputChannel)

	server0 := "54.197.93.172:8030"
	server1 := "54.81.223.113:8030"
	server2 := "34.201.252.188:8030"
	server3 := "54.226.178.220:8030"
	server4 := "54.175.180.75:8030"
	server5 := "3.83.207.68:8030"
	server6 := "18.212.65.185:8030"
	server7 := "18.212.197.115:8030"

	addrPtr := [8]string{server0, server1, server2, server3, server4, server5, server6, server7}

	client := make([]*rpc.Client, p.Threads)
	for i := 0; i < p.Threads; i++ {
		client[i], _ = rpc.Dial("tcp", addrPtr[i])
		defer client[i].Close()
	}

	operationType := noOperation
	writeDone := make(chan bool)
	go manageSdl(&operationType, writeDone, c.keyPresses, p, world, c)

	executeTurns(world, p, c, turn, &operationType, client, writeDone)

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
