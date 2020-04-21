# Checking `cliquematch` for memory leaks #

This directory contains a sample program and `Makefile` that runs the core algorithm of `cliquematch`.
It uses `valgrind` to check if the search method for cliques causes any memory leaks. 
Python/`pybind11` is not required.

## Usage ##

1. Clone the `cliquematch` repo
2. Download the graph (`cond-mat-2003.mtx`)[https://sparse.tamu.edu/Newman/cond-mat-2003] to this folder
3. Run `make` to compile the sample program in `main.cpp`.
4. Run `make test` which calls `valgrind` on the sample program twice (once with the heuristic, once without).
5. Check that `valgrind` does not report any memory leaks.
6. Check that both cases find a clique of size 25 : `9986 9987 10066 10068 10071 10072 10074 10076 10077 10078 10079 10080 10081 10082 10083 10085 10287 10902 10903 10904 10905 10906 10907 10908 10909`


