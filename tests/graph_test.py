# -*- coding: utf-8 -*-
"""
    cliquematch.tests.graph_test
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Testcases for cliquematch.cm_base.Graph
"""

import pytest
import cliquematch
import numpy as np
import random


class TestGraph(object):

    """
    Uses pytest to:
    test basic properties of cliquematch.cm_base.Graph

    * various methods of initialization
    * setting and accessing attributes
    * ensuring dfs works for a small sample Graph
    * (doesn't check continue_search or heuristic)
    """

    def test_adjmat(self):
        adjmat = np.zeros((5, 5), dtype=np.bool)
        adjmat[0, 1:] = True
        adjmat[1, [0, 3]] = True
        adjmat[2, [0, 4]] = True
        adjmat[3, [0, 1]] = True
        adjmat[4, [0, 2]] = True
        G1 = cliquematch.Graph(adjmat)

        assert G1.n_vertices == 5
        assert G1.n_edges == np.sum(adjmat) / 2
        assert G1.adjacency_list == [
            set(),
            {2, 3, 4, 5},
            {1, 4},
            {1, 5},
            {1, 2},
            {1, 3},
        ]

        badmat = np.zeros((5, 4), dtype=np.bool)
        with pytest.raises(RuntimeError):
            G2 = cliquematch.Graph(badmat)

    def test_elist(self):
        elist = np.array(
            [[1, 2], [1, 3], [1, 4], [1, 5], [2, 4], [3, 5]], dtype=np.uint32
        )
        G1 = cliquematch.Graph(elist, 5)
        assert G1.n_vertices == 5
        assert G1.n_edges == len(elist)
        assert G1.adjacency_list == [
            set(),
            {2, 3, 4, 5},
            {1, 4},
            {1, 5},
            {1, 2},
            {1, 3},
        ]

        elist[0, 0] = 0
        print(elist)
        with pytest.raises(RuntimeError):
            G2 = cliquematch.Graph(elist, 5)
        elist[0, 0] = 1

        with pytest.raises(RuntimeError):
            G2 = cliquematch.Graph(elist, 3)

    def test_filing(self):
        G1 = cliquematch.Graph("./tests/sample_read1.mtx", 1)
        assert G1.n_vertices == 5
        assert G1.n_edges == 6
        assert G1.adjacency_list == [
            set(),
            {2, 3, 4, 5},
            {1, 4},
            {1, 5},
            {1, 2},
            {1, 3},
        ]

        G2 = cliquematch.Graph("./tests/sample_read2.mtx", 2)
        assert G2.n_vertices == 5
        assert G2.n_edges == 6
        assert G2.adjacency_list == [
            set(),
            {2, 3, 4, 5},
            {1, 4},
            {1, 5},
            {1, 2},
            {1, 3},
        ]

    def test_data(self):
        edges = np.array(
            [
                [2, 3],
                [1, 3],
                [1, 4],
                [1, 5],
                [1, 6],
                [1, 7],
                [4, 5],
                [4, 6],
                [4, 7],
                [5, 6],
                [5, 7],
                [6, 7],
                [2, 8],
                [3, 8],
            ],
            dtype=np.uint32,
        )

        G = cliquematch.Graph(edges, 8)
        G.use_dfs = True
        G.use_heuristic = False
        G.size_limit = 100
        G.time_limit = 200
        full_inf = repr(G)
        temp_inf = str(G)
        read_onlys = [
            G.current_vertex,
            G.search_done,
            G.n_vertices,
            G.n_edges,
            G.adjacency_list,
        ]

        with pytest.raises(AttributeError):
            G.current_vertex = 25

        with pytest.raises(AttributeError):
            G.search_done = False

        with pytest.raises(AttributeError):
            G.n_vertices = 22

        with pytest.raises(AttributeError):
            G.n_edges = 31

        with pytest.raises(AttributeError):
            G.adjacency_list = []

    def test_dfs(self):
        edges = np.array(
            [
                [2, 3],
                [1, 3],
                [1, 4],
                [1, 5],
                [1, 6],
                [1, 7],
                [4, 5],
                [4, 6],
                [4, 7],
                [5, 6],
                [5, 7],
                [6, 7],
                [2, 8],
                [3, 8],
            ],
            dtype=np.uint32,
        )

        G = cliquematch.Graph(edges, 8)
        G.use_dfs = True
        G.use_heuristic = False
        ans = G.get_max_clique()
        assert ans == [1, 4, 5, 6, 7]
