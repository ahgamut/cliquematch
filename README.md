# `cliquematch`
## Finding correspondence via maximum cliques in large graphs

[![pyvers](https://img.shields.io/badge/python-3.5+-blue.svg)][3]
[![license](https://img.shields.io/github/license/ahgamut/cliquematch)][4]
[![travis build](https://travis-ci.com/ahgamut/cliquematch.svg?branch=master)][5]
[![appveyor build](https://ci.appveyor.com/api/projects/status/27r2qy8mbog04bhg?svg=true)][6]
[![DOI](https://zenodo.org/badge/196044254.svg)][7]

The `cliquematch` package aims to do two specific things:

1.  Find [maximum cliques][wiki] in large sparse undirected graphs, as quickly and efficiently as possible.
	(`cliquematch` uses C++ internally to implement a really fast maximum clique algorithm).

2.  Construct large sparse undirected graphs in-memory for the various applications of the maximum
	clique/clique enumeration problem.

Licensed under MIT License. View the documentation at https://cliquematch.readthedocs.io/  
Note: this is `cliquematch v2`. If you're looking for `v1`, you can go to the [`v1` branch][v1] or
[documentation][v1docs].


## Installation Instructions

### Installing from a wheel

[PyPI][wheels] wheels are available for Linux and Windows. 

```bash
pip install cliquematch>=2.1.0
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

[1]: https://github.com/pybind/pybind11/
[2]: https://gitlab.com/libeigen/eigen/-/releases#3.3.7
[3]: https://www.python.org/download/releases/3.5.0/
[4]: https://github.com/ahgamut/cliquematch/blob/master/LICENSE
[5]: https://travis-ci.com/ahgamut/cliquematch
[6]: https://ci.appveyor.com/project/ahgamut/cliquematch
[7]: https://zenodo.org/badge/latestdoi/196044254
[wiki]: https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions
[wheels]: https://pypi.org/project/cliquematch/
[v1]: https://www.github.com/ahgamut/cliquematch/tree/v1/
[v1docs]: https://www.cliquematch.readthedocs.io/en/v1/
