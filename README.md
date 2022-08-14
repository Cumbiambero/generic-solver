# General purpose solver

The main idea is to create a solver that is able to guess a possible formula for any given list of results.
An evolutionary approach will be used.

The project has not yet reached a usable state.

## Actual implementation

### First draft
- Tree structure
- Formula representation

### In progress
- Solver
- Creators
- Changers
- Input definition (two CSV files)

### Not yet started
- Input processing
- Parallelization

## Usage
### Installation
```
mkdir build && cd build
cmake ..
make
```

### Run
```
./solver input.csv results.csv x
```

Copyright 2022, Eduardo Hahn Paredes