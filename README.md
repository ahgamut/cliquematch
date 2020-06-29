# `cliquematch`
## Finding correspondence via maximum cliques in large graphs

[![pyvers](https://img.shields.io/badge/python-3.5+-blue.svg)][3]
[![license](https://img.shields.io/github/license/ahgamut/cliquematch)][4]
[![travis build](https://travis-ci.com/ahgamut/cliquematch.svg?branch=master)][5]
[![appveyor build](https://ci.appveyor.com/api/projects/status/27r2qy8mbog04bhg?svg=true)][6]

The `cliquematch` package is used to to find a [maximum clique][wiki] in large sparse undirected graphs as
quickly as possible. It also provides a framework with generic classes for implementing applications of the
maximum clique problem: finding a (sub)set of corresponding elements between two sets *S*<sub>1</sub> and
*S*<sub>2</sub>.

## Installation Instructions

### Installing from a wheel

PyPI wheels will be available as soon I figure out the build+test+publish process.

### Installing from source

1. `cliquematch` requires [`pybind11`][1] (v2.2 or newer) for its setup: 

```bash
pip3 install pybind11
```
2. `cliquematch` requires [`Eigen`][2] (v3.3.7 or newer) as part of its setup. 
	
	* You can clone the repo via `git clone --recursive` to get [`Eigen`][2].
	* If you already have an existing version of [`Eigen`][2], or have downloaded it separately,
	set the `EIGEN_DIR` environment variable to the folder containing [`Eigen`][2] before compilation.

3. A `C++11` compatible compiler must be available for the installation:

	* On Linux, gcc is called with `--std=c++11` (builds with `gcc 4.8.2` for `manylinux1` wheels). 
	* On Windows, Visual Studio 2015 Update 3 (MSVC 14.0 runtime) or later is needed.
	* **Note:** Installing under Windows+MinGW has not been tested.

4. Compilation Flags: `setup.py` compiles the `cliquematch` extension with two additional flags.
	
	* `STACK_DFS` (`1` by default): If nonzero, `cliquematch` uses an explicit stack for the depth-first clique
	  search;  otherwise it uses recursive function calls. Primarily for debugging purposes.

	* `BENCHMARKING` (`0` by default): Set to `1` when benchmarking the core cliquematch algorithm.

[1]: https://github.com/pybind/pybind11/
[2]: https://gitlab.com/libeigen/eigen/-/releases#3.3.7
[3]: https://www.python.org/download/releases/3.5.0/
[4]: https://github.com/ahgamut/cliquematch/blob/master/LICENSE
[5]: https://travis-ci.com/ahgamut/cliquematch
[6]: https://ci.appveyor.com/project/ahgamut/cliquematch
[wiki]: https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions
