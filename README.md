# Conway's Game of Life

A C++ implementation of Conway's Game of Life, a cellular automaton devised by mathematician John Conway.

## About

The Game of Life is a zero-player simulation where cells on a grid evolve according to a set of simple rules. Despite its simplicity, the system can produce complex and fascinating patterns.

## Rules

For each generation:

1. Any live cell with fewer than two live neighbors dies (underpopulation).
2. Any live cell with two or three live neighbors survives.
3. Any live cell with more than three live neighbors dies (overpopulation).
4. Any dead cell with exactly three live neighbors becomes a live cell (reproduction).

## Features

* Grid-based simulation
* Generation updates based on Conway's rules
* Console-based visualization
* Written in modern C++

## Project Structure

```
.
├── src/
│   └── main.cpp
├── MakeFile
├── bin/
└── obj/
```

## Build

Compile the project using:

```bash
make
```

## Run

```bash
./bin/game_of_life
```

On Windows:

```bash
bin\game_of_life.exe
```

## Future Improvements

* Custom grid sizes
* Pattern loading from files
* Colored terminal output
* Graphical user interface
* Simulation speed controls

## Author

Avi G
