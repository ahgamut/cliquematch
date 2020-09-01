User Guide
==========

As described in the homepage, `cliquematch` aims to do two things:

1. Find `maximum cliques`_ in large sparse undirected graphs, as quickly and efficiently as possible. 

2. Construct large sparse undirected graphs in-memory for the various
   applications of the maximum clique/clique enumeration problem.
  
Let's look at how `cliquematch` can be used as described above.


Finding a Maximum Clique
------------------------

Finding a maximum clique in an undirected graph is a well known problem.  Most
exact algorithms, including the one used in `cliquematch`, use some form of a
depth-first search (DFS), along with some pruning techniques to speed up the
search.

Finding a maximum clique using `cliquematch` is simple (and *fast*): load a
`~cliquematch.Graph` from a file (or from an adjacency matrix, an adjacency
list, or a list of edges), set some search properties, and find a maximum
clique in it. 

For example, we load a graph from a file:
`cond-mat-2003.mtx <https://sparse.tamu.edu/Newman/cond-mat-2003>`__
and find a maximum clique.

.. code:: python

    import cliquematch
    G = cliquematch.Graph.from_file("cond-mat-2003.mtx")

We can also load a `~cliquematch.Graph` using an adjacency matrix
(`~cliquematch.Graph.from_adj_matrix`), an adjacency list
(`~cliquematch.Graph.from_adj_list`), or a list of edges
(`~cliquematch.Graph.from_edgelist`).  Once the `~cliquematch.Graph` object has
been loaded, we can provide various parameters to search for a clique with `~cliquematch.Graph.get_max_clique`:

.. code:: python

    answer = G.get_max_clique(
        lower_bound=1,
        upper_bound=1729,
        time_limit=100,
        use_heuristic=True,
        use_dfs=True,
    )
    print(answer)
    # [9986, 9987, 10066, 10068, 10071, 10072, 10074, 10076,
    # 10077, 10078, 10079, 10080, 10081, 10082, 10083, 10085,
    # 10287, 10902, 10903, 10904, 10905, 10906, 10907, 10908, 10909]

* A ``lower_bound`` and an ``upper_bound`` can be set for the size for clique.
* A ``time_limit`` can be set for the clique search.
* In case the graph is very large, to get a large clique quickly via a
  heuristic method, ``use_heuristic`` can be set to `True`, and
  ``use_dfs`` can be set to `False` to skip the depth-first
  search.

.. warning::
    Both ``use_heuristic`` and ``use_dfs`` cannot be set to `False`.
    `cliquematch` will then skip the clique search entirely and raise a `RuntimeError`.

The `~cliquematch.Graph` object has some read-only properties:

* `~cliquematch.Graph.n_vertices` and `~cliquematch.Graph.n_edges` provide the
  number of vertices and edges in the graph.
* `~cliquematch.Graph.search_done` is `True` if the depth-first search has been completed.

The user can print the `~cliquematch.Graph` object to view the properties:

.. code:: python 
    
    print(G)
    # cliquematch.core.Graph object at 0x559e7da730c0
    # (n_vertices=31163,n_edges=120029,search_done=False)


What is the `~cliquematch.Graph.search_done` property for? In case the clique
search was interrupted due to ``time_limit``\, the user can provide a
``continue_search`` parameter within a loop like this:

.. code:: python

    while not G.search_done:
        answer = G.get_max_clique(
            lower_bound=1,
            upper_bound=1729,
            time_limit=100,
            use_heuristic=True,
            use_dfs=True,
            continue_search=True,
        )

If the entire `~cliquematch.Graph` needs to be searched again (say for a clique of larger size),
the user can call the `~cliquematch.Graph.reset_search` method:

.. code:: python

    G.reset_search()
    G.get_max_clique(
        lower_bound=1, upper_bound=31, time_limit=100, use_heuristic=True, use_dfs=True
    )


Finding Multiple cliques : clique enumeration
---------------------------------------------

The above `~cliquematch.Graph` methods deal with finding *one* large clique. A
related use case is to find *multiple* large cliques in a given graph,and
iterate through them in some order.

The `~cliquematch.Graph` class has an `~cliquematch.Graph.all_cliques` method
for finding all cliques of a given size. Taking the `~cliquematch.Graph` object
loaded as per the above section:

.. code:: python
   
   import cliquematch
   G = cliquematch.Graph.from_file("cond-mat-2003.mtx")
   # we know there exists a maximum clique of size 25
   # so let's find cliques of size 24
   for clique in G.all_cliques(size=24):
        print(clique)


Applications of the maximum clique problem  
------------------------------------------

Applications of the maximum clique problem primarily involve:

1. the construction of a graph from a different kind of dataset, 
2. writing the graph to a file, 
3. reading the file again, 
4. finding a maximum clique, and then 
5. conversion of the clique back into the existing dataset. 
   
This process is usually repeated with tweaks to underlying dataset, leading to
different graphs and cliques.  For such use cases the primary bottlenecks are
the construction of the graph, reading/processing the graph data in a
clique-friendly manner, and finding the maximum clique. `cliquematch` aims to
solve these issues by keeping the graph construction in memory, and having an
optimized clique search algorithm.

Graph construction for maximum clique problems mostly involve one of the two ways below:

* A graph is constructed using an *edge indication function* on all pairs of
  elements belonging to a dataset :math:`X`. 
  
    * `cliquematch` does not provide any specific code for this; an edge list
      can constructed from the data using a nested loop, following which a
      `~cliquematch.Graph` object can be loaded,and the maximum clique (ie the
      largest group of related elements in the dataset :math:`X`) can be computed.

* A *correspondence graph* is constructed using the elements of *two* datasets
  :math:`P` and :math:`Q`; the vertices of the graph refer to pairs of elements
  :math:`(p_i, q_j)`, and an edge between two vertices implies some common
  relationship between the elements from :math:`P` and the elements from :math:`Q`.

Yeah... that's a little dense. Let's try again with some math. 

Assume you have two sets :math:`P = { p_1, p_2, ... p_M }` and :math:`Q = {
q_1, q_2, ... q_N }`, and we want to find the **largest** subsets :math:`P^* \in
P` and :math:`Q^* \in Q` such that there exists a *one-to-one correspondence*
between :math:`P^*` and :math:`Q^*`. 

This means the elements of :math:`P^*` are related to each other similar to how
the elements of :math:`Q^*` are related to each other. Suppose the elements of
:math:`P^*` (and similarly :math:`Q^*`) have a *pairwise* relationship, then we
can say that for all pairs

.. math::
   ( (p_{i_1}, p_{i_2}), (q_{j_1}, q_{j_2}) ) \in P^* \times P^* \times Q^* \times Q^* \\
   i_1 \neq i_2 \\
   j_1 \neq j_2 \\
   

there exists some boolean *condition function* :math:`f` such that`

.. math::
    
   f(p_{i_1}, p_{i_2}, q_{j_1}, q_{j_2}) = 1 \\
   \forall
   ( p_{i_1}, p_{i_2}, q_{j_1}, q_{j_2} ) \in P^* \times P^* \times Q^* \times Q^* \\
   i_1 \neq i_2 \\
   j_1 \neq j_2 \\


What does this have to do with maximum cliques? Well, :math:`P^*` and
:math:`Q^*` are the **largest** such subsets, so maybe finding them can be done
by converting the problem to a maximum clique problem. This is where we bring
in a *correspondence graph*: an undirected graph :math:`G(V,E)`, where :math:`V
= P \times Q` ie the vertices indicate a mapping :math:`(p_i, q_j)`. As for edges,
that's where :math:`f` comes in: an edge exists between :math:`v_1 = (p_{i_1},
q_{j_1})` and :math:`v_2 = (p_{i_2}, q_{j_2})` if and only if:


.. math::
    
   f(p_{i_1}, p_{i_2}, q_{j_1}, q_{j_2}) = 1


It can be proved that finding a maximum clique in the correspondence graph
:math:`G` is the same as finding the largest subsets :math:`P^*` and
:math:`Q^*` that have a one-to-one correspondence.


`cliquematch` provides classes the implement the above functionality of
correspondence graphs. The user has to provide are the sets :math:`P` and
:math:`Q`, along with a condition function :math:`f`.  A common use case is to
have :math:`f` expressed using *distance metrics*:

.. math::

   f(p_{i_1}, p_{i_2}, q_{j_1}, q_{j_2}) = 1 \iff || d_P(p_{i_1}, p_{i_2}) - d_Q(q_{j_1}, q_{j_2}) || \leq \epsilon
 
The correspondence graph classes are all subclasses of `~cliquematch.Graph`, they all expose the same methods:

* An ``__init__`` method that accepts the sets :math:`P` (or ``S1``),
  :math:`Q` (or ``S2``), and the distance functions for ``S1`` and ``S2``
  respectively.
* A `~cliquematch.A2AGraph.build_edges` method that constructs the correspondence graph using only the distance metrics.
* A `~cliquematch.A2AGraph.build_edges_with_condition` method that accepts a condition function ``cf``, and a boolean ``use_condition_only``:
  
    * If ``use_cfunc_only`` is `True`, the graph is constructed using only ``cf`` (slower)
    * Otherwise the graph is constructed using the distance metrics and pruned with ``cf`` (faster)

* A `~cliquematch.A2AGraph.get_correspondence` method that returns the largest
  corresponding subsets :math:`P^*` and :math:`Q^*`, or the indices of the
  elements in the subsets.

* A `~.cliquematch.A2AGraph.all_correspondences` method similar to the
  `cliquematch.Graph.all_cliques` that works similar to
  `~cliquematch.A2AGraph.get_correspondence`.

The correspondence graph classes available are:

* `cliquematch.A2AGraph` where ``S1`` and ``S2`` are 2-D `numpy.ndarray`\ s
* `cliquematch.L2LGraph` where ``S1`` and ``S2`` are `list`\ s (or any list-like object)
* `cliquematch.A2LGraph` where ``S1`` is a 2-D `numpy.ndarray` and ``S2`` is a `list`
* `cliquematch.L2AGraph` where ``S2`` is a `list` and ``S2`` is a 2-D `numpy.ndarray`
* `cliquematch.IsoGraph` where ``S1`` and ``S2`` are `~cliquematch.Graph`\ s (subgraph isomorphism).
* `cliquematch.AlignGraph` which is a special case of `~cliquematch.A2AGraph` used for image alignment.

The concept of correspondence graphs enables applying maximum cliques to many
fields. Here are a couple of examples from the `Github repo`_:

1. |ccmm| can be implemented using `cliquematch` |ccmm_impl| .
2. |molec| can be implemented |molec_impl| .


.. _maximum cliques: https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions
.. _Github repo: https://github.com/ahgamut/cliquematch
.. |ccmm| replace:: `This image matching algorithm <https://link.springer.com/article/10.1007/s10489-015-0646-1>`__
.. |ccmm_impl| replace:: `like this <https://github.com/ahgamut/cliquematch/blob/master/examples/ccmm.py>`__
.. |molec| replace:: `Simple molecular alignment <https://www.sciencedirect.com/science/article/abs/pii/S1093326397000892>`__
.. |molec_impl| replace:: `like this <https://github.com/ahgamut/cliquematch/blob/master/examples/molecule.py>`__
