# -*- coding: utf-8 -*-
"""
    cliquematch.tests.test_iso
    ~~~~~~~~~~~~~~

    testcases for cliquematch.IsoGraph

    :license: see LICENSE for more details.
"""
import pytest
import numpy as np
import cliquematch


class TestIsoGraph(object):

    """
    Test out properties of IsoGraph using the graph in the
    sample .mtx file 
    
    * Loading properly
    * Testing that edges can be built
    * testing data access and dfs (too small for heuristic)
    """

    S1 = cliquematch.Graph("./tests/sample_read1a.mtx", 1).adjacency_list
    S2 = cliquematch.Graph("./tests/sample_read1b.mtx", 1).adjacency_list

    def test_loading(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)

    def test_edge_build(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)
        G.build_edges()

    def test_data(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)

        G.epsilon = 0.1
        G.S2[0] = set()
        G.S1[0] = set()
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
        G = cliquematch.IsoGraph(self.S1, self.S2)
        G.epsilon = 0.001
        G.build_edges()
        G.use_dfs = True
        ans = G.get_correspondence(return_indices=True)
        assert set(ans[1]) == set([0, 1, 2, 3, 4])

        ans2 = G.get_correspondence(return_indices=False)
        assert ans2[1] == self.S2
