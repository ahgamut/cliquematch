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
import os

def get_location(fname):
    return os.path.join(os.path.dirname(os.path.abspath(__file__)), fname)

class TestIsoGraph(object):

    """
    Test out properties of IsoGraph using the graph in the
    sample .mtx file

    * Loading properly
    * Testing that edges can be built
    * testing data access and dfs (too small for heuristic)
    """

    S1 = cliquematch.Graph.from_file(get_location("sample_read1a.mtx"))
    S2 = cliquematch.Graph.from_file(get_location("sample_read1b.mtx"))

    def test_loading(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)

    def test_edge_build(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)
        G.build_edges()

    def test_dfs(self):
        G = cliquematch.IsoGraph(self.S1, self.S2)
        G.build_edges()
        ans = G.get_correspondence(return_indices=True)
        assert set(ans[0]) == {1, 2, 3, 4, 5}
        assert ans[1] == [2, 1, 3, 4, 5]
        g1, g2, mapping = G.get_correspondence(return_indices=False)
        for k in mapping.keys():
            assert len(g1[k]) == len(g2[mapping[k]])
            t = set(mapping[x] for x in g1[k])
            assert t == g2[mapping[k]]
