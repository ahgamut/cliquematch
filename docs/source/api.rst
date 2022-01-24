API Documentation
=================

.. py:module:: cliquematch

.. py:class:: Graph()

    .. py:attribute:: search_done

        Whether the search has been completed (Readonly)

        :type: `bool`

    .. py:attribute:: n_vertices

        Number of vertices in the graph (Readonly)

        :type: `int`

    .. py:attribute:: n_edges

        Number of edges in the graph (Readonly)

        :type: `int`

    .. py:method:: get_max_clique

        Finds a maximum clique in graph within the given bounds
        
        :param `int` lower_bound: 
                set a lower bound on the clique size. default is **1**.
        :param `int` upper_bound: 
                set an upper bound on the clique size. default is **65535**.
        :param `float` time_limit: 
                set a time limit for the search: a nonpositive value implies there is no time limit
                (use in conjunction with ``continue_search``\ ). default is **-1**.
        :param `bool` use_heuristic:
                if `True`\, use the heuristic-based search to obtain a large clique quickly.
                Good for obtaining an initial lower bound. default is `True`.
        :param `bool` use_dfs:
                if `True`\, use the depth-first to obtain the clique. default is `True`\.
        :param `bool` continue_search:
                set as `True` to continue a clique search interrupted by ``time_limit``\.
                default is `False`\.
        :returns: the vertices in the maximum clique
        :rtype: `list`
        :raises RuntimeError: if the graph is empty or a clique could not be found

    .. py:method:: reset_search

        Reset the search space for `~cliquematch.Graph.get_max_clique`.
        
        :raises RuntimeError: if the graph is empty

    .. py:method:: all_cliques(size)

        Iterate through all cliques of a given size in the `~cliquematch.Graph`.

        :param `int` size: size of a clique to search for.
        :rtype: `~cliquematch.core.CliqueIterator`
        :raises RuntimeError: if the graph is empty

    .. py:staticmethod:: from_file

        Constructs `~cliquematch.Graph` instance from reading a Matrix Market file

        :param str filename:
        :returns: the loaded `~cliquematch.Graph`
        :raises RuntimeError: if the file could not be read

    .. py:staticmethod:: from_edgelist

        Constructs `~cliquematch.Graph` instance from the given edge list.
        Note that vertex indices must start from 1.

        :param `numpy.ndarray` edgelist: shape ``(n,2)``\
        :param `int` num_vertices: 
        :returns: the loaded `~cliquematch.Graph`
        :raises RuntimeError: if any value in ``edgelist`` is greater than ``num_vertices``
        :raises RuntimeError: if value in ``edgelist`` is ``0``

    .. py:staticmethod:: from_matrix

        Constructs `~cliquematch.Graph` instance from the given boolean adjacency matrix

        :param `numpy.ndarray` adjmat: `bool` square matrix
        :returns: the loaded `~cliquematch.Graph`
        :raises RuntimeError: if ``adjmat`` is not square or the edges could not be constructed

    .. py:staticmethod:: from_adjlist

        Constructs `~cliquematch.Graph` instance from the given adjacency list.
        Note that the first element of the list must be an empty `set`\, vertex
        indices start at 1.

        :param `int` num_vertices:
        :param `int` num_edges:
        :param `list` edges: `list` of `set`\s
        :returns: the loaded `~cliquematch.Graph`
        :raises RuntimeError: if first element in ``edges`` is nonempty, or there are invalid vertices/edges

    .. py:method:: to_file

        Exports `~cliquematch.Graph` instance to a Matrix Market file
        
        :param str filename:
        :raises RuntimeError: if the file could not be opened, or if the graph is empty


    .. py:method:: to_edgelist

        Exports `~cliquematch.Graph` instance to an edge list

        :returns: ``(n,2)`` `numpy.ndarray` of edges
        :raises RuntimeError: if the graph is empty

    .. py:method:: to_matrix

        Exports `~cliquematch.Graph` instance to a boolean matrix
        
        :returns: square `numpy.ndarray` of `bool`\ s
        :raises RuntimeError: if the graph is empty

    .. py:method:: to_adjlist

        Exports `~cliquematch.Graph` instance to an adjacency list
        
        :returns: `list` of `set`\ s
        :raises RuntimeError: if the graph is empty

.. py:class:: NWGraph()

    .. py:attribute:: search_done

        Whether the search has been completed (Readonly)

        :type: `bool`

    .. py:attribute:: n_vertices

        Number of vertices in the graph (Readonly)

        :type: `int`

    .. py:attribute:: n_edges

        Number of edges in the graph (Readonly)

        :type: `int`

    .. py:method:: get_max_clique

        Finds a maximum clique in graph within the given bounds
        
        :param `float` lower_bound: 
                set a lower bound on the clique size. default is **1**.
        :param `float` upper_bound: 
                set an upper bound on the clique size. default is **65535**.
        :param `bool` use_heuristic:
                if `True`\, use the heuristic-based search to obtain a large clique quickly.
                Good for obtaining an initial lower bound. default is `True`.
        :param `bool` use_dfs:
                if `True`\, use the depth-first to obtain the clique. default is `True`\.
        :returns: the vertices in the maximum clique
        :rtype: `list`
        :raises RuntimeError: if the graph is empty or a clique could not be found

    .. py:method:: reset_search

        Reset the search space for `~cliquematch.NWGraph.get_max_clique`.
        
        :raises RuntimeError: if the graph is empty

    .. py:method:: all_cliques(size)

        Iterate through all cliques of a given size in the `~cliquematch.NWGraph`.

        :param `float` size: size of a clique to search for.
        :rtype: `~cliquematch.core.NWCliqueIterator`
        :raises RuntimeError: if the graph is empty

    .. py:method:: get_clique_weight(clique)

        Return the weight of the provided clique.

        :param `list` clique: a clique obtained via `~cliquematch.NWGraph.get_max_clique`.
        :rtype: `float`

    .. py:staticmethod:: from_edgelist

        Constructs `~cliquematch.NWGraph` instance from the given edge list.
        Note that vertex indices must start from 1.

        :param `numpy.ndarray` edgelist: shape ``(n,2)``\
        :param `int` num_vertices: 
        :returns: the loaded `~cliquematch.NWGraph`
        :raises RuntimeError: if any value in ``edgelist`` is greater than ``num_vertices``
        :raises RuntimeError: if value in ``edgelist`` is ``0``

    .. py:staticmethod:: from_matrix

        Constructs `~cliquematch.NWGraph` instance from the given boolean adjacency matrix

        :param `numpy.ndarray` adjmat: `bool` square matrix
        :returns: the loaded `~cliquematch.NWGraph`
        :raises RuntimeError: if ``adjmat`` is not square or the edges could not be constructed

    .. py:staticmethod:: from_adjlist

        Constructs `~cliquematch.NWGraph` instance from the given adjacency list.
        Note that the first element of the list must be an empty `set`\, vertex
        indices start at 1.

        :param `int` num_vertices:
        :param `int` num_edges:
        :param `list` edges: `list` of `set`\s
        :returns: the loaded `~cliquematch.NWGraph`
        :raises RuntimeError: if first element in ``edges`` is nonempty, or there are invalid vertices/edges

    .. py:method:: to_edgelist

        Exports `~cliquematch.NWGraph` instance to an edge list

        :returns: ``(n,2)`` `numpy.ndarray` of edges
        :raises RuntimeError: if the graph is empty

    .. py:method:: to_matrix

        Exports `~cliquematch.NWGraph` instance to a boolean matrix
        
        :returns: square `numpy.ndarray` of `bool`\ s
        :raises RuntimeError: if the graph is empty

    .. py:method:: to_adjlist

        Exports `~cliquematch.NWGraph` instance to an adjacency list
        
        :returns: `list` of `set`\ s
        :raises RuntimeError: if the graph is empty


.. autoclass:: A2AGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: L2LGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: L2AGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: A2LGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: IsoGraph(set1, set2)
.. autoclass:: AlignGraph(set1, set2)
   :no-inherited-members:

.. py:class:: _WrappedIterator

   Wrapper for a `~cliquematch.core.CorrespondenceIterator` object to
   provide results as per ``return_indices``. Satisfies the `iter`\/`next` protocol.

.. py:module:: cliquematch.core

.. py:class:: CliqueIterator
   
   A class satisfying the `iter`\/`next` protocol to produces cliques of a given size from a `~cliquematch.Graph`.

.. py:class:: CorrespondenceIterator
   
   A class satisfying the `iter`\/`next` protocol to produces correspondences of a given size from a `~cliquematch.Graph`.

.. py:class:: NWCliqueIterator
   
   A class satisfying the `iter`\/`next` protocol to produces cliques of a given weight from a `~cliquematch.NWGraph`.
