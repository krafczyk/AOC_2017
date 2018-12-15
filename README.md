# AOC_2017

These are my solutions to [Advent of Code 2017](https://adventofcode.com/2017)!

## Building

Simply use `make` to build all the task binaries. The only prerequisite is a compiler. By default I'm using gcc. You can change this by modifying the Makefile.

## Running

For a given task, the given test data from the problem description along with any other tests I felt were useful to test whether the code was working are in the directory `tests`. The problem data are all located in the directory 'problems'.

Generally speaking, to run a test or problem set for a given task, execute something like the following:

`./bin/task_N -i problems/task_N.txt`

Most problems also use the `-v` flag to give verbose output.
