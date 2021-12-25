# -*- coding: utf-8 -*-
"""
    cliquematch.test_align
    ~~~~~~~~~~~~~~~~~~~~~~

    testcases for cliquematch.AlignGraph

    :copyright: (c) 2020 by Gautham Venkatasubramanian.
    :license: see LICENSE for more details.
"""
import pytest
import numpy as np
import cliquematch
import random


class TestAlignGraph(object):
    """
    Test out properties of AlignGraph using random numpy arrays

    AlignGraph is essentially the same as A2AGraph, so we only

    * test that edges can be built using control points and a mask
    * test dfs and heuristic work correctly using above construction
    """

    np.random.seed(824)
    mask = np.ones((200, 200), dtype=np.bool_)
    S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
    S2 = np.float64(np.random.uniform(0, 100, (20, 2)))

    def test_dfs(self):
        S1 = self.S1
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S1_sub = S1[subset, :]
        S2 = np.float64(np.matmul(S1_sub, rotmat) + [100, 100])
        print(S1_sub, S2)
        G = cliquematch.AlignGraph(S1, S2)
        G.epsilon = 0.001
        G.build_edges_with_filter(S2, self.mask, percentage=0.8)
        ans = G.get_correspondence(
            use_heuristic=False, use_dfs=True, upper_bound=10, return_indices=False
        )

        assert abs(ans["theta"] - np.pi / 3) < 1e-5
        assert abs(ans["dx"] - 100) < 1e-5
        assert abs(ans["dy"] - 100) < 1e-5

    def test_heuristic(self):
        S1 = self.S1
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S1_sub = S1[subset, :]
        S2 = np.float64(np.matmul(S1_sub, rotmat) + [100, 100])
        G = cliquematch.AlignGraph(S1, S2)
        G.epsilon = 0.001
        G.build_edges_with_filter(S2, self.mask, percentage=0.8)
        ans = G.get_correspondence(
            use_heuristic=True, use_dfs=False, upper_bound=10, return_indices=False
        )
        print(G)

        assert abs(ans["theta"] - np.pi / 3) < 1e-5
        assert abs(ans["dx"] - 100) < 1e-5
        assert abs(ans["dy"] - 100) < 1e-5

    def test_filter(self):
        S1 = self.S1
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S1_sub = S1[subset, :]
        S2 = np.float64(np.matmul(S1_sub, rotmat) + [100, 100])
        G = cliquematch.AlignGraph(S1, S2)
        G.epsilon = 0.001
        G.build_edges_with_filter(S2, self.mask, percentage=0.8)
        ans = G.get_correspondence(
            use_heuristic=False, use_dfs=True, upper_bound=10, return_indices=False
        )
        print(G)

        G2 = cliquematch.AlignGraph(S1, S2)
        G2.epsilon = 0.001
        filt = cliquematch.wrappers.aligngraph.MaskFilter(S2, self.mask, percentage=0.8)
        G2.build_edges_with_condition(filt, False)
        ans2 = G2.get_correspondence(
            use_dfs=True, use_heuristic=False, upper_bound=10, return_indices=False
        )
        print(G2)
        assert abs(ans["theta"] - ans2["theta"]) < 1e-8
        assert abs(ans["dx"] - ans2["dx"]) < 1e-8
        assert abs(ans["dy"] - ans2["dy"]) < 1e-8
