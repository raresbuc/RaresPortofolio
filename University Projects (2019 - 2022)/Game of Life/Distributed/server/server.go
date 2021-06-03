package main

import (
	"flag"
	"fmt"
	"net"
	"net/rpc"
	"os"
	"strconv"
)

const alive = 255
const dead = 0

func mod(x, m int) int {
	return (x + m) % m
}

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

func calculateNeighbours(height, width, x, y int, world [][]uint8) int {
	neighbours := 0
	for i := -1; i <= 1; i++ {
		for j := -1; j <= 1; j++ {
			if i != 0 || j != 0 {
				if world[mod(y+i, height)][mod(x+j, width)] == alive {
					neighbours++
				}
			}
		}
	}
	return neighbours
}

func NextIteration(world [][]uint8, bound int) [][]uint8 {
	boundOffset := 0
	if bound != 0 {
		boundOffset = 1
	}

	height := len(world)
	width := len(world[0])

	var golChange [][]uint8
	golChange = make([][]byte, height)
	for i := range golChange {
		golChange[i] = make([]byte, width)
	}

	fmt.Println("Height: " + strconv.Itoa(height))
	fmt.Println("Width: " + strconv.Itoa(width))
	fmt.Println()

	for y := 0 + boundOffset; y < height-boundOffset; y++ {
		for x := 0; x < width; x++ {
			golChange[y][x] = 0
			currentNeighbours := calculateNeighbours(height, width, x, y, world)

			if world[y][x] == alive {
				if currentNeighbours < 2 || currentNeighbours > 3 {
					golChange[y][x] = 1
				}
			} else {
				if currentNeighbours == 3 {
					golChange[y][x] = 1
				}
			}
		}
	}
	return golChange
}

func handleError(err error) {
	if err != nil {
		fmt.Println("error")
	}
}

type WorldOperation struct{}

func (w *WorldOperation) GetNextIteration(req Request, res *Response) (err error) {
	res.GolChange = NextIteration(req.GolWorld, req.Bound)
	return
}

func (w *WorldOperation) StopServer(req RequestStop, res *ResponseStop) (err error) {
	if req.Message == "k" {
		os.Exit(0)
	}
	return
}

func main() {
	pAddr := flag.String("port", "8030", "Port to listen on")
	flag.Parse()
	rpc.Register(&WorldOperation{})
	listener, _ := net.Listen("tcp", ":"+*pAddr)
	defer listener.Close()
	rpc.Accept(listener)
}
