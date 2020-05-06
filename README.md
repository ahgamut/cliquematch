# `cliquematch` : Finding cliques in large sparse graphs

The `cliquematch` package is used to to find a [maximum
clique](https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions) in large sparse undirected graphs as
quickly as possible. It also provides a framework with generic classes for implementing applications of the
maximum clique problem: finding a (sub)set of corresponding elements between two sets *S*<sub>1</sub> and
*S*<sub>2</sub>.

## Installation Instructions

### Installing from a wheel

### Installing from source

1. `cliquematch` requires `pybind11` for its setup: 

```bash
pip3 install pybind11
```
2. `cliquematch` requires Eigen (v3.3.7 or newer) as part of its setup. 
	
	* You can clone the repo via `git clone --recursive` to get Eigen.
	* If you already have an existing version of Eigen, set the `EIGEN_DIR` 
	environment variable to folder containing Eigen before compilation.

3. A `C++-14` compatible compiler must be available on `PATH` for the installation:

	* On Linux, GCC is called with `--std=c++14`. 
	* On Windows, Visual Studio 2015 (MSVC 14.0 runtime) or later is needed.
	* **Note:** Installing under Windows+MinGW is untested and may crash.

4. Compilation Flags: `setup.py` compiles the `cliquematch` extension with two additional flags.
	
	* `STACK_DFS` (`1` by default): If nonzero, `cliquematch` uses an explicit stack for the depth-first clique
	  search;  otherwise it uses recursive function calls. Primarily for debugging purposes.

	* `BENCHMARKING` (`0` by default): Set to `1` when benchmarking the core cliquematch algorithm.
