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
    mask = np.ones((200, 200), dtype=np.bool)
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
        # transpose rotmat so that rotation is actually by pi/3
        S2 = np.float64(np.matmul(S1_sub, rotmat.T) + [100, 100])
        print(S1_sub, S2)
        G = cliquematch.AlignGraph(S1, S2)
        G.epsilon = 0.001
        G.time_limit = 100
        G.use_heuristic = False
        G.use_dfs = True
        G.upper_bound = 10
        G.build_edges_with_filter(S1_sub, self.mask, percentage=0.8)
        ans = G.get_correspondence()

        subset.sort()
        assert ans[0].tolist() == subset

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
        # transpose rotmat so that rotation is actually by pi/3
        S2 = np.float64(np.matmul(S1_sub, rotmat.T) + [100, 100])
        G = cliquematch.AlignGraph(S1, S2)
        G.epsilon = 0.001
        G.time_limit = 100
        G.use_heuristic = True
        G.use_dfs = True
        G.upper_bound = 10
        G.build_edges_with_filter(S1_sub, self.mask, percentage=0.8)
        ans = G.get_correspondence()
        print(G)
        subset.sort()
        assert set(ans[0].tolist()) <= set(subset)
