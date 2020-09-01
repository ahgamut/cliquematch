`cliquematch`
=============

Finding correspondence via maximum cliques in large graphs
----------------------------------------------------------

|pyvers| |license| |travis build| |appveyor build|

The `cliquematch` package aims to do two specific things:
 
1. Find a `maximum clique`_ in large sparse undirected graphs, as quickly and
   efficiently as possible. (`cliquematch` uses C++ internally to implement a
   really fast maximum clique algorithm).

2. Construct large sparse undirected graphs in-memory for the various
   applications of the maximum clique problem.

That's it. `cliquematch` does not provide a way to modify a large graph once
it has been loaded, or any other general capability, because: (1) the internal
data structures are designed to optimize the clique search, (2) sparse graphs
constructed for applications of the maximum clique problem are rarely modified
by hand (constructed anew instead), and (3) there are better
packages (|networkx|_ and |igraph|_) for general graph analysis.

.. note::

    `cliquematch` version 2 is available: it has a simpler API, and also
    implements clique enumeration. View version 2 at the `master branch`_ or
    `docs`_.

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
   :target: https://github.com/ahgamut/cliquematch/blob/v1/LICENSE
.. |travis build| image:: https://travis-ci.com/ahgamut/cliquematch.svg?branch=v1
   :target: https://travis-ci.com/ahgamut/cliquematch
.. |appveyor build| image:: https://ci.appveyor.com/api/projects/status/27r2qy8mbog04bhg?svg=true
   :target: https://ci.appveyor.com/project/ahgamut/cliquematch
.. _maximum clique: https://en.wikipedia.org/wiki/Clique_(graph_theory)#Definitions
.. |networkx| replace:: ``networkx``
.. _networkx: https://networkx.github.io/documentation/stable/
.. |igraph| replace:: ``igraph``
.. _igraph: https://igraph.org/python/
.. _master branch: https://github.com/ahgamut/cliquematch/
.. _docs: https://cliquematch.readthedocs.io/en/stable
