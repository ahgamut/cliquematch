`cliquematch`
=============

Finding correspondence via maximum cliques in large graphs
----------------------------------------------------------

|pyvers| |license| |ci build| |doi|

The `cliquematch` package aims to do two specific things:
 
1. Find `maximum cliques`_ in large sparse undirected graphs, as quickly and
   efficiently as possible. `cliquematch` v3 also enables finding
   vertex-weighted maximum cliques via the `~cliquematch.NWGraph` class.

2. Construct large sparse undirected graphs in-memory for the various
   applications of the maximum clique/clique enumeration problem.

That's it. `cliquematch` does not provide a way to modify a large graph once
it has been loaded, or any other general capability, because:

* the internal data structures are designed to optimize the clique search
* sparse graphs constructed for applications of the maximum clique problem are
  rarely modified by hand (constructed anew instead)
* there are better packages (|networkx|_ and |igraph|_) for general graph analysis.


.. note::
    
   This is `cliquematch` v3, which has a simpler API than v1 and also
   provides the capability of clique enumeration. You view the v1 docs `here`_.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   setup.rst
   user.rst
   api.rst
   how.rst


.. |pyvers| image:: https://img.shields.io/badge/python-3.5+-blue.svg
   :target: https://www.python.org/download/releases/3.5.0/
.. |license| image:: https://img.shields.io/github/license/ahgamut/cliquematch
   :target: https://github.com/ahgamut/cliquematch/blob/master/LICENSE
.. |ci build| image:: https://github.com/ahgamut/cliquematch/actions/workflows/wheels.yml/badge.svg
   :target: https://github.com/ahgamut/cliquematch/actions
.. |doi| image:: https://zenodo.org/badge/196044254.svg
   :target: https://zenodo.org/badge/latestdoi/196044254
.. _maximum cliques: https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions
.. |networkx| replace:: ``networkx``
.. _networkx: https://networkx.github.io/documentation/stable/
.. |igraph| replace:: ``igraph``
.. _igraph: https://igraph.org/python/
.. _here: https://cliquematch.readthedocs.io/en/v1
