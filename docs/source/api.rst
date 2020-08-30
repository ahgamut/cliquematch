API Documentation
=================

.. py:module:: cliquematch

.. py:class:: cliquematch.Graph()

    .. py:attribute:: use_heuristic

        Search using the heuristic if true

        :type: `bool`

    .. py:attribute:: use_dfs

        Perform the depth-first search if true

        :type: `bool`

    .. py:attribute:: lower_bound

        Set the lower bound on the size of clique to find

        :type: `int`

    .. py:attribute:: upper_bound

        Set the upper bound on the size of clique to find

        :type: `int`

    .. py:attribute:: time_limit

        Set the time limit on the search

        :type: `float`

    .. py:attribute:: current_vertex

        The vertex which is about to be searched (Readonly)

        :type: `int`

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
        
        :returns: the vertices in the maximum clique
        :rtype: `list`

    .. py:method:: continue_search

        Continue the clique search if the entire graph has not been searched

    .. py:method:: reset_search

        Reset the search for maximum cliques

    .. py:staticmethod:: from_file

        Constructs `Graph` instance from reading a Matrix Market file

        :param str: filename
        :returns: the loaded `Graph`

    .. py:staticmethod:: from_edgelist

        Constructs `Graph` instance from the given edge list

        :param: edgelist (`numpy.ndarray` of shape ``(n,2)``\ )
        :param `int`: num_vertices
        :returns: the loaded `Graph`

    .. py:staticmethod:: from_matrix

        Constructs `Graph` instance from the given boolean adjacency matrix

        :param: adjmat (`numpy.ndarray`\ , `bool` square matrix)
        :returns: the loaded `Graph`

    .. py:staticmethod:: from_adjlist

        Constructs `Graph` instance from the given adjacency list

        :param `int`: num_vertices
        :param `int`: num_edges
        :param `list`: edges
        :returns: the loaded `Graph`

    .. py:method:: to_file

        Exports `Graph` instance to a Matrix Market file
        
        :param str: filename

    .. py:method:: to_edgelist

        Exports `Graph` instance to an edge list

        :returns: ``(n,2)`` `numpy.ndarray` of edges

    .. py:method:: to_matrix

        Exports `Graph` instance to a boolean matrix
        
        :returns: square `numpy.ndarray` of `bool`\ s

    .. py:method:: to_adjlist

        Exports `Graph` instance to an adjacency list
        
        :returns: `list` of `set`\ s

.. autoclass:: A2AGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: L2LGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: L2AGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: A2LGraph(set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True)
.. autoclass:: IsoGraph(set1, set2)
.. autoclass:: AlignGraph(set1, set2)
   :no-inherited-members:

