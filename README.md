# `cliquematch` : Finding cliques in large sparse graphs

The `cliquematch` package is used to to find a [maximum clique](https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions)
in large sparse undirected graphs as quickly as possible. Additionally, the functionality of `cliquematch` extends to finding a (sub)set of
corresponding points between two sets *S*<sub>1</sub> and *S*<sub>2</sub> and thus provides a method of aligning these point sets.


## Installation Instructions

### Installing from a wheel

Windows- and Linux-compatible `.whl` files should be available, just download the right one and run: 

```
pip install cliquematch_<python_version>_<os>.whl
```

### Installing from source

`cliquematch` requires `pybind11` for its setup: run `pip install pybind11` first 
(you may need admin privileges to install pybind11 correctly).    
A `C++-14` compatible compiler must be available on `PATH` for the installation; the package was tested on `g++` in Ubuntu 
and MSVC 14.0 (or) MinGW's `g++` on Windows. 

Download the latest source version of `cliquematch.tar.gz` and run: 

```
pip install cliquematch_<version>.tar.gz
``` 

**Note:** This package has not been tested on MacOS. However, there are no (explicit) requirements that would prevent
 this.

## Finding cliques in a graph

Consider a large sparse undirected graph *G* in which we need to find a
maximum clique. The graph *G* can be loaded in the following ways:

```python
import numpy as np
import cliquematch

# number of vertices, followed by edge list
# V E
# v1 v2
# ..
G = cliquematch.graph("path/to/file.mtx", 1)


# number of vertices, followed by weighted edge list
# V E
# v1 v2 0.5
# .. 
G = cliquematch.graph("path/to/file.mtx", 1)

# from an adjacency matrix (2D matrix of numpy booleans)
G = cliquematch.graph(adjmat)

# from a list of edges in memory (numpy Nx2 matrix, and no of vertices V)
G = cliquematch.graph(edge_list, n_vertices)
```

We shall consider the first method.  
Let’s download a graph from the [SuiteSparse Matrix Collection](https://sparse.tamu.edu/SNAP):
[ca-AstroPh](https://sparse.tamu.edu/SNAP/ca-AstroPh).

```python
# download ca-AstroPh.tar.gz and extract ca-AstroPh.mtx

G = cliquematch.graph("/absolute/path/to/ca-AstroPh.mtx", 1)
print(G)
```

Now let’s find a maximum clique in this graph. First, let’s set a few
parameters:

-   I want to find the largest clique possible
-   I want to find a clique within 100 seconds
-   I want to use the heuristic to search
-   I want to perform the depth-first search as well

```python
G.size_limit = 100000
G.time_limit = 100
G.use_heuristic = True
G.use_dfs = True

max_clique = G.get_max_clique() 
print(max_clique)
```

Alternatively, if you would like to operate with smaller time bounds and find large cliques incrementally, i.e.

- I just want to find a large clique in this graph
- I want to check the obtained clique every 0.1 seconds

```python
G.time_limit = 0.1
while not G.search_done:
    G.continue_search()
    print("I have searched through %d vertices in the graph"%(G.current_vertex))
    max_clique = G.get_max_clique()
    print(max_clique)
    
    if len(max_clique) > some_bound_I_have:
        print("Good enough")
        break
```

## Finding correspondence between point sets

`cliquematch` can also be used to find a correspondence between two sets
of points *S*<sub>1</sub> and *S*<sub>2</sub>.

In this simple example, we construct set *S*<sub>1</sub>, and consider a
transformed subset of it as *S*<sub>2</sub>, and load them into a graph
*G*:

```python
s1 = np.random.uniform(0, 100, (200,2))
s1_sub = np.random.choice(s1, (50,2), replace=False)

rotation = np.array([ [np.cos(np.pi/3), -np.sin(np.pi/3)], [np.sin(np.pi/3), np.cos(np.pi/3)]])
translation = np.array([1,1])
s2 = np.matmul(s1_sub, rotation) + translation

G = psgraph(s1, s2)

```

Note that the vertex set of *G* is merely *S*<sub>1</sub> × *S*<sub>2</sub>. Now we can set the strictness for
constructing the edges in the graph *G*.

There is an edge between (*p*<sub>*i*</sub>, *q*<sub>*i*</sub>) and (*p*<sub>*j*</sub>, *q*<sub>*j*</sub>) if and only if the below
condition is satisfied: ||*d*(*p*<sub>*i*</sub>, *p*<sub>*j*</sub>) − *d*(*q*<sub>*i*</sub>, *q*<sub>*j*</sub>)|| ≤ *ϵ*  

Hence we need to set the `epsilon` parameter in *G*, before we build the correspondence graph. 
**Warning:** A large value of `epsilon` will result in a dense correspondence graph, which will take much longer to construct.


```python
G.epsilon = 0.1
G.build_edges()
```

Now that the edges of the graph have been constructed, we can find the set of corresponding points between
 *S*<sub>1</sub> and *S*<sub>2</sub>. This is equivalent to finding the maximum clique in *G*: 

```python
G.size_limit = 100000
G.time_limit = 100
G.use_heuristic = True
G.use_dfs = True

corr = G.get_correspondence() # this actually calls G.get_max_clique() and then does some processing
print(corr)
```

I can also find correspondences incrementally, just use a `psgraph` and replace `get_max_clique()` 
with `get_correspondence()`.


We can use the obtained correspondence to align the sets *S*<sub>1</sub> and *S*<sub>2</sub> such that 
the points of interest have maximum overlap. This is currently done via the [Kabsch
algorithm](https://en.wikipedia.org/wiki/Kabsch_algorithm) to obtain a rigid transformation, but other non-linear 
transforms (polynomial, thin-plate splines) can be used as well.

Additionally, if *S*<sub>1</sub> and *S*<sub>2</sub> are <u>points obtained from an image</u>, the correspondence can be used to align
these images. (Install `cliquematch_gui` for a convenient user interface to align images via point correspondence).
