Using `cliquematch` in R
================

While [`cliquematch`](https://github.com/ahgamut/cliquematch) is a
Python package, it can also be called from R by using
[`reticulate`](https://rstudio.github.io/reticulate/). This document
provides R snippets similar to the Python snippets in the `cliquematch`
[documentation](https://cliquematch.readthedocs.io/en/stable/user.html).

We can set up our python environment from within RStudio by loading
`reticulate` and selecting the python interpreter to use. I use the
[`use_condaenv`](https://rstudio.github.io/reticulate/reference/use_python.html)
function select a Python interpreter, but you can use `use_python` as
well.

``` r
library(reticulate)
use_condaenv(condaenv = "cm-dev")
# make sure you have installed cliquematch in python
cliquematch = import("cliquematch")
```

## Finding a maximum clique

Load a graph from a file:
[cond-mat-2003.mtx](https://sparse.tamu.edu/Newman/cond-mat-2003) and
find a maximum clique.

``` r
G = cliquematch$Graph$from_file("./cond-mat-2003.mtx")
print(G)
```

    ## cliquematch.core.Graph(n_vertices=31163,n_edges=120029,search_done=True)

We can also load a `Graph` using an adjacency matrix
(`Graph$from_adj_matrix`), an adjacency list (`Graph$from_adj_list`), or
a list of edges (`Graph$from_edgelist`). Once the `Graph` object has
been loaded, we can provide various parameters to search for a clique
with `Graph$get_max_clique`:

``` r
answer = G$get_max_clique(lower_bound=1L,upper_bound=1729L, 
                          time_limit=100, use_heuristic=T,
                          use_dfs=T, continue_search=F)
print(answer)
```

    ##  [1]  9986  9987 10066 10068 10071 10072 10074 10076 10077 10078 10079 10080
    ## [13] 10081 10082 10083 10085 10287 10902 10903 10904 10905 10906 10907 10908
    ## [25] 10909

  - A `lower_bound` and an `upper_bound` can be set for the size for
    clique.
  - A `time_limit` can be set for the clique search. If time limits are
    not needed, set `0` as the limit.
  - The `continue_search` parameter can be set to `TRUE` if the clique
    search terminated due to a time limit earlier.
  - In case the graph is very large, to get a large clique quickly via
    aheuristic method, `use_heuristic` can be set to `True`, and
    `use_dfs` can be set to `False` to skip the depth-first search.

### Continuing an interrupted clique search

``` r
while (!G$search_done) {
    answer = G$get_max_clique(lower_bound=1L,upper_bound=1729L, 
                              time_limit=100, use_heuristic=T, 
                              use_dfs=T, continue_search=T)
    # process answer if needed
}
print(answer)
```

    ##  [1]  9986  9987 10066 10068 10071 10072 10074 10076 10077 10078 10079 10080
    ## [13] 10081 10082 10083 10085 10287 10902 10903 10904 10905 10906 10907 10908
    ## [25] 10909

## Clique enumeration

To obtain all cliques of a particular size: use the `all_cliques`
method. Use
[`iterate`](https://rstudio.github.io/reticulate/reference/iterate.html)
or
[`iter_next`](https://rstudio.github.io/reticulate/reference/iterate.html)
to view the cliques one by one:

``` r
cliques = G$all_cliques(size = 25L)
# iterate(cliques, f = print)
clq = iter_next(cliques, completed = NULL)
i = 0
while(!is.null(clq))
{
    
    print(clq)
    clq = iter_next(cliques, completed = NULL)
    i = i + 1
}
```

    ##  [1]  9986  9987 10066 10068 10071 10072 10074 10076 10077 10078 10079 10080
    ## [13] 10081 10082 10083 10085 10287 10902 10903 10904 10905 10906 10907 10908
    ## [25] 10909

Thus the `cond-mat-2003` graph contains exactly one clique of size 25.
