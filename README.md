# General purpose solver

The main idea is to create a solver that is able to guess a possible formula for any given list of results.
An evolutionary approach is used.

The project has not yet reached a usable state.

## Usage

### Installation

```
mkdir build && cd build
cmake ..
make
```

### Run

A csv file containing the values for the input and a csv file representing the corresponding results have to be passed
as commandline arguments. Finally variable names have to be defined.

```
./solver input.csv results.csv x
```

### Examples

There are some input and result files stored in the test subdirectory as demonstration:

```
./solver ../test/resources/input-one-to-ten.csv ../test/resources/square-results.csv x
./solver ../test/resources/input-one-to-ten.csv ../test/resources/square-plus-three-results.csv x
./solver ../test/resources/input-one-to-thousand.csv ../test/resources/primes-results.csv x
./solver ../test/resources/input-one-to-ten.csv ../test/resources/circle-results.csv r
```

### Development

To build and run the tests set -DBUILD_TESTING=ON when running CMake.

Copyright 2025, Eduardo Hahn Paredes