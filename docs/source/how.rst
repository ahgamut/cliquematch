How `cliquematch` works
=======================

`cliquematch` implements an exact algorithm to find maximum cliques, with a lot
of pruning optimizations for large sparse graphs.  The algorithm used is
similar to FMC :cite:`fmc` and PMC :cite:`pmc`, but also uses the concept of
bitset-compression from BBMC :cite:`bbmc` to save memory.  It is implemented in
``C++11``, does not use any additional libraries apart from the ``C++`` STL,
and is single-threaded.

Avoiding memory allocations
---------------------------

In earlier versions of `cliquematch`, the clique search methods were
occasionally slowed due to many requests of small amounts from heap memory. In
version 2.1, `cliquematch` does exactly *one* heap allocation -- a
``std::vector`` large enough to hold a maximum clique -- at the start of every
search method, be it the heuristic search, finding one clique or enumerating
cliques. During the clique search, it performs *zero* memory allocations. After
a clique is found, it requests memory to return the clique to the user. Thus
the number of heap allocations is mostly constant: a set number of allocations
to initialize the `Graph` object (changes slightly depending on size of the
graph), zero allocations during the search, and one allocation per clique
returned.

This is possible because clique size always has an upper bound (degree,
core-number, or the measure used in `cliquematch` all provide such a bound) and
because bitsets are used for representing branches of the clique search. Thus
`cliquematch` computes an upper bound for clique size (ie how deep a search can
go), the maximum bitset space required for a vertex ( :math:`\lceil
\frac{d_{max}}{64} \rceil` ) and allocates space accordingly. The allocated
space is reused throughout the search, thus avoiding any heap allocations
during the most used part of the program. 

Benchmarking the `cliquematch` algorithm
----------------------------------------

The below table shows how cliquematch compares to FMC and PMC.

+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| :math:`name`       | :math:`V`   | :math:`E`   | :math:`w`   | :math:`t_{cm}`   | :math:`t_{fmc}`   | :math:`t_{pmc}`   | :math:`t_{cm\_heur}`   | :math:`w_{cm\_heur}`   | :math:`t_{fmc\_heur}`   | :math:`w_{fmc\_heur}`   |
+====================+=============+=============+=============+==================+===================+===================+========================+========================+=========================+=========================+
| Erdos02            | 6927        | 8472        | 7           | **0.0001**       | 0.0015            | 0.0022            | 0.0001                 | 6                      | 0.0009                  | 7                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| Erdos972           | 5488        | 7085        | 7           | **0.0001**       | 0.0004            | 0.0016            | 0.0001                 | 7                      | 0.0002                  | 6                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| Erdos982           | 5822        | 7375        | 7           | **0.0001**       | 0.0004            | 0.0018            | 0.0                    | 7                      | 0.0002                  | 7                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| Erdos992           | 6100        | 7515        | 8           | **0.0001**       | 0.0004            | 0.0018            | 0.0                    | 8                      | 0.0002                  | 8                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| Fault\_639         | 638802      | 14626683    | 18          | **2.0622**       | 13.8244           | -                 | 0.5864                 | 18                     | 2.4625                  | 18                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| brock200\_2        | 200         | 9876        | 12          | 0.217            | 0.6421            | **0.0016**        | 0.0019                 | 10                     | 0.0023                  | 9                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| c-fat200-5         | 200         | 8473        | 58          | 0.0023           | 0.4169            | **0.0003**        | 0.0001                 | 58                     | 0.0102                  | 58                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| ca-AstroPh         | 18772       | 198110      | 57          | **0.0004**       | 0.0789            | 0.0132            | 0.0003                 | 56                     | 0.0277                  | 57                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| ca-CondMat         | 23133       | 93497       | 26          | **0.0003**       | 0.0048            | 0.0083            | 0.0002                 | 26                     | 0.0042                  | 26                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| ca-GrQc            | 5242        | 14496       | 44          | **0.0001**       | 0.0005            | 0.0017            | 0.0001                 | 43                     | 0.0012                  | 44                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| ca-HepPh           | 12008       | 118521      | 239         | **0.0034**       | 0.0131            | 0.0149            | 0.0012                 | 239                    | 0.2518                  | 239                     |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| ca-HepTh           | 9877        | 25998       | 32          | **0.0001**       | 0.0007            | 0.0035            | 0.0                    | 32                     | 0.0001                  | 32                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| caidaRouterLevel   | 192244      | 609066      | 17          | **0.0257**       | 0.2098            | 0.0774            | 0.0084                 | 17                     | 0.073                   | 15                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| coPapersCiteseer   | 434102      | 16036720    | 845         | **0.0248**       | 0.8344            | 1.4019            | 0.0144                 | 845                    | 14.6734                 | 845                     |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| com-Youtube        | 1134890     | 2987624     | 17          | **1.1393**       | 9.618             | -                 | 0.1412                 | 16                     | 0.3515                  | 13                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| cond-mat-2003      | 31163       | 120029      | 25          | **0.0007**       | 0.0118            | 0.0096            | 0.0004                 | 25                     | 0.0038                  | 25                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| cti                | 16840       | 48232       | 3           | **0.0014**       | 0.0036            | 0.0046            | 0.0009                 | 3                      | 0.0013                  | 3                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| hamming6-4         | 64          | 704         | 4           | **0.0002**       | 0.0003            | 8.5115            | 0.0                    | 4                      | 5.6982                  | 4                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| johnson8-4-4       | 70          | 1855        | 14          | 0.0405           | 0.1459            | **0.0003**        | 0.0001                 | 11                     | 0.0005                  | 14                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| keller4            | 171         | 9435        | 11          | **3.4486**       | 15.4211           | -                 | 0.0016                 | 9                      | 0.0027                  | 9                       |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+
| loc-Brightkite     | 58228       | 214078      | 37          | 5.0651           | 2.7146            | **0.0277**        | 0.0038                 | 36                     | 0.0151                  | 31                      |
+--------------------+-------------+-------------+-------------+------------------+-------------------+-------------------+------------------------+------------------------+-------------------------+-------------------------+

-  ``fmc -t 0 -p`` was used to run the FMC branch-and-bound algorithm.

-  ``pmc -t 1 -r 1 -a 0 -h 0 -d`` (single CPU thread, reduce wait time
   of 1 second, full algorithm, skip heuristic, search in descending
   order) was used to run the PMC branch-and-bound algorithm.

-  ``fmc -t 1`` was used to run the FMC heuristic algorithm.

-  A small script was used to run the `cliquematch` algorithm. `cliquematch` was compiled with ``BENCHMARKING=1``.



The benchmark graphs were taken from the UFSparse :cite:`ufsparse`, SNAP
:cite:`snap`, and DIMACS :cite:`dimacs` collections.  :math:`|V|` and
:math:`|E|` denote the number of nodes and edges in the graph. :math:`\omega`
denotes the size of the maximum clique found. All the branch-and-bound methods
agreed on the maximum clique size in every benchmark. :math:`t_{cm}`,
:math:`t_{fmc}`, and :math:`t_{pmc}` denote the time taken by `cliquematch`,
FMC, and PMC respectively in the branch-and-bound search: the least time is in
bold text.  :math:`w_{cm-heur}`, :math:`t_{cm-heur}` denote the size of clique
and time taken by the heuristic method in `cliquematch`; and similarly for
:math:`w_{fmc-heur}` and :math:`t_{fmc-heur}`. A minus sign (``-``) indicates
that the program returned an error without completing the calculation.

Correspondence Graphs in ``C++``
--------------------------------

The correspondence graph classes are generated using simple C++ template
programming (simple because it's basically a typed macro substitution).
`cliquematch` can be extended with custom correspondence graphs: they can be
prototyped using the existing classes, and/or implemented in C++ for
performance. The source code of `~cliquematch.IsoGraph` or
`~cliquematch.AlignGraph` are good places to start if you're trying to implement
a custom correspondence graph class.


References
^^^^^^^^^^

.. bibliography:: refs.bib
   :style: unsrt
