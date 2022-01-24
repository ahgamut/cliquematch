# -*- coding: utf-8 -*-
"""
    cliquematch.tests.nwgraph_test
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Testcases for cliquematch.NWGraph
"""

import pytest
import cliquematch
import numpy as np
import os


def get_location(fname):
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), fname)


class TestNWGraph(object):

    """
    Uses pytest to:
    test basic properties of cliquematch.NWGraph

    * various methods of initialization
    * setting and accessing attributes
    * ensuring dfs works for a small sample NWGraph
    * checks that the search can be reset
    """

    def test_adjmat(self):
        adjmat = np.zeros((5, 5), dtype=np.bool_)
        weights = np.ones((5,), dtype=np.float64)
        adjmat[0, 1:] = True
        adjmat[1, [0, 3]] = True
        adjmat[2, [0, 4]] = True
        adjmat[3, [0, 1]] = True
        adjmat[4, [0, 2]] = True
        G1 = cliquematch.NWGraph.from_matrix(adjmat, weights)

        assert G1.n_vertices == 5
        assert G1.n_edges == np.sum(adjmat) / 2
        assert np.sum(adjmat ^ G1.to_matrix()) == 0

        badmat = np.zeros((5, 4), dtype=np.bool_)
        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_matrix(badmat, weights)

        with pytest.raises(RuntimeError):
            G3 = cliquematch.NWGraph.from_matrix(adjmat, weights[:-1])

    def test_elist(self):
        elist = np.array(
            [[1, 2], [1, 3], [1, 4], [1, 5], [2, 4], [3, 5]], dtype=np.uint32
        )
        weights = np.ones((5,), dtype=np.float64)
        G1 = cliquematch.NWGraph.from_edgelist(elist, 5, weights)
        assert G1.n_vertices == 5
        assert G1.n_edges == len(elist)
        assert np.sum(G1.to_edgelist() - elist) == 0

        elist[0, 0] = 0
        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_edgelist(elist, 5, weights)
        elist[0, 0] = 1

        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_edgelist(elist, 3, weights)

        with pytest.raises(RuntimeError):
            G3 = cliquematch.NWGraph.from_edgelist(elist, 5, weights[:-1])

    def test_adjlist(self):
        adjlist = [
            set(),
            {2, 3, 4, 5},
            {1, 4},
            {1, 5},
            {1, 2},
            {1, 3},
        ]
        weights = np.full((5,), 2.7, dtype=np.float64)
        G1 = cliquematch.NWGraph.from_adjlist(5, 6, adjlist, weights)
        assert G1.n_vertices == 5
        assert G1.n_edges == 6
        assert G1.to_adjlist() == adjlist

        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_adjlist(4, 6, adjlist, weights)

        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_adjlist(5, 7, adjlist, weights)

        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_adjlist(5, 6, adjlist, weights[1:])

        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_adjlist(4, 6, adjlist[1:], weights)

        adjlist[1].add(1)
        with pytest.raises(RuntimeError):
            G2 = cliquematch.NWGraph.from_adjlist(5, 6, adjlist, weights)

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
        weights = np.full((8,), 3.1, dtype=np.float64)

        G = cliquematch.NWGraph.from_edgelist(edges, 8, weights)
        full_inf = repr(G)
        temp_inf = str(G)
        read_onlys = [
            G.search_done,
            G.n_vertices,
            G.n_edges,
        ]

        with pytest.raises(AttributeError):
            G.search_done = False

        with pytest.raises(AttributeError):
            G.n_vertices = 22

        with pytest.raises(AttributeError):
            G.n_edges = 31

        assert G._get_vertex_weights() == [3.1] * 8

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
        weights = np.full((8,), 3.1, dtype=np.float64)

        G = cliquematch.NWGraph.from_edgelist(edges, 8, weights)
        ans = G.get_max_clique(use_heuristic=False, use_dfs=True)
        assert ans == [1, 4, 5, 6, 7]
        assert G.get_clique_weight(ans) == 15.5

    def test_reset_search(self):
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
        weights = np.full((8,), 1.1, dtype=np.float64)

        G = cliquematch.NWGraph.from_edgelist(edges, 8, weights)
        with pytest.raises(RuntimeError):
            ans = G.get_max_clique(
                lower_bound=6, upper_bound=32, use_dfs=True, use_heuristic=False
            )
        G.reset_search()
        ans = G.get_max_clique(
            lower_bound=1, upper_bound=32, use_dfs=True, use_heuristic=False
        )
        assert ans == [1, 4, 5, 6, 7]

    def test_enumi(self):
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
        weights = np.full((8,), 0.1, dtype=np.float64)

        G = cliquematch.NWGraph.from_edgelist(edges, 8, weights)
        ans = G.get_max_clique(lower_bound=0, use_heuristic=False, use_dfs=True)
        assert ans == [1, 4, 5, 6, 7]

        # checking for size = 1 doesn't make sense anymore
        c2 = list(x for x in G.all_cliques(size=0.2))
        assert len(c2) == G.n_edges
        c4 = list(x for x in G.all_cliques(size=0.4))
        assert len(c4) == 5
        for x in c4:
            assert set(x) < set(ans)
