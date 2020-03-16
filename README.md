# `cliquematch` : Finding cliques in large sparse graphs

The `cliquematch` package is used to to find a [maximum clique](https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions)
in large sparse undirected graphs as quickly as possible. Additionally, the functionality of `cliquematch` extends to finding a (sub)set of
corresponding elements between two sets *S*<sub>1</sub> and *S*<sub>2</sub>.


## Installation Instructions

### Installing from a wheel

### Installing from Github

1. `cliquematch` requires `pybind11` for its setup: 

```bash
pip install pybind11
```
You may need admin privileges to install pybind11 correctly.

2. `cliquematch` requires Eigen (v3.3.7 or newer) as part of its setup. 
	
	* You can clone the repo via `git clone --recursive` to get Eigen.
	* Or if you already have an existing version of Eigen, set the `EIGEN_DIR` 
	environment variable before compilation.

3. A `C++-14` compatible compiler must be available on `PATH` for the installation:

	* On Linux, `gcc` is called with `--std=c++14`. 
	* On Windows, Visual Studio 2015 (MSVC 14.0 runtime) or later is needed.
	* **Note:** Installing under Windows+MinGW is untested and may crash.
