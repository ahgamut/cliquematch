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

## Usage

`cliquematch` is used for loading a large graph and finding a maximum clique in it. 
For example, we load [`cond-mat-2003.mtx`][cond2003], and find a maximum clique.

```python
import cliquematch
G = cliquematch.Graph.from_file("cond-mat-2003.mtx")
G.time_limit = 1
print(G)
# cliquematch.core.Graph object at 0x559e7da730c0
# (n_vertices=31163,n_edges=120029,lower_bound=1,upper_bound=4294967295,
# time_limit=1,use_heuristic=False,use_dfs=True,search_done=False)
G.get_max_clique()
# [9986, 9987, 10066, 10068, 10071, 10072, 10074, 10076,
# 10077, 10078, 10079, 10080, 10081, 10082, 10083, 10085,
# 10287, 10902, 10903, 10904, 10905, 10906, 10907, 10908, 10909]
```

The search can be tuned in terms of size/time bounds, and reset if necessary. 
If required, `use_heuristic` can be set to `True` to find a large clique quickly.

### Correspondence graphs

Many applications of maximum cliques involve construction of a correspondence graph to find corresponding subsets
between two given sets. `cliquematch` also contains classes for correspondence graphs:

* [This image matching algorithm][ccmm] can be implemented using `cliquematch` [like this][ccmmapp].
* [Simple molecular alignment][molecule] can be implemented [like this][moleculeapp].

The correspondence graph classes are generated using C++ template programming.  `cliquematch` can be extended
with custom correspondence graphs: they can be prototyped using the existing classes, and/or implemented in
C++ for performance.

## Installation Instructions

### Installing from a wheel

PyPI wheels are available for Linux and Windows. 
MacOS builds are tested but wheels are not provided.

```bash
pip install cliquematch
```

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
[test]: https://test.pypi.org/project/cliquematch/
[cond2003]: https://sparse.tamu.edu/Newman/cond-mat-2003
[ccmm]: https://link.springer.com/article/10.1007/s10489-015-0646-1
[ccmmapp]: https://github.com/ahgamut/cliquematch/blob/master/examples/ccmm.py
[molecule]: https://www.sciencedirect.com/science/article/abs/pii/S1093326397000892
[moleculeapp]: https://github.com/ahgamut/cliquematch/blob/master/examples/molecule.py
