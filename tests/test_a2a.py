# -*- coding: utf-8 -*-
"""
    cliquematch.tests.test_a2a
    ~~~~~~~~~~~~~~~~~~~~~~~~~~

    testcases for cliquematch.A2AGraph

    :license: see LICENSE for more details.
"""

import pytest
import numpy as np
import cliquematch
import random
import warnings


def eucd(mat, i, j):
    ans = np.sqrt(np.sum((mat[i, :] - mat[j, :]) * (mat[i, :] - mat[j, :])))
    # print(mat[i], mat[j], ans)
    return ans


class dummy(object):
    def __init__(self, N):
        self.N = N

    def __call__(self, mat, i, j):
        # print(i, j, "ndarray")
        return np.power(np.sum(np.abs(mat[i, :] - mat[j, :]) ** self.N), 1 / self.N)


class dummy2(object):
    def __init__(self, N, eps=0.0001):
        self.N = N
        self.eps = eps

    def __call__(self, mat1, i1, j1, mat2, i2, j2):
        ans1 = np.power(np.sum(np.abs(mat1[i1, :] - mat1[j1, :]) ** self.N), 1 / self.N)
        ans2 = np.power(np.sum(np.abs(mat2[i2, :] - mat2[j2, :]) ** self.N), 1 / self.N)
        ans = np.abs(ans1 - ans2)
        # print(i1, j1, i2, j2, ans <= self.eps)
        return ans < self.eps


class TestA2AGraph(object):
    """
    Test out properties of A2AGraph using random numpy arrays

    * Loading with different callables (and warning default as Euclidean)
    * testing that edges can be built, with or without a cfunc
    * testing data access, dfs, and heuristic
    * testing time limits with and continue search
    """

    np.random.seed(824)
    S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
    S2 = np.float64(np.random.uniform(0, 100, (20, 2)))

    def test_loading(self):
        S1 = self.S1
        S2 = self.S2
        with pytest.warns(UserWarning):
            G1 = cliquematch.A2AGraph(S1, S2)
        with pytest.warns(UserWarning):
            G2 = cliquematch.A2AGraph(S1, S2, d1=eucd)
        ds1 = dummy(2)
        ds2 = dummy(3)
        G3 = cliquematch.A2AGraph(
            S1, S2, d1=ds1, d2=ds2, is_d1_symmetric=True, is_d2_symmetric=True
        )
        G3 = cliquematch.A2AGraph(
            S1, S2, d1=ds1, d2=eucd, is_d1_symmetric=True, is_d2_symmetric=False
        )
        G3 = cliquematch.A2AGraph(
            S1, S2, d1=eucd, d2=ds2, is_d1_symmetric=False, is_d2_symmetric=True
        )
        with pytest.warns(UserWarning):
            G3 = cliquematch.A2AGraph(
                S1, S2, d1=None, d2=ds1, is_d1_symmetric=False, is_d2_symmetric=False
            )

    def test_edge_build(self):
        S1 = self.S1
        S2 = self.S2
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        S2 = S1[subset, :]
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.00001
        G.build_edges()

    def test_edge_custom(self):
        S1 = self.S1
        S2 = self.S2
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        S2 = S1[subset, :]
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        cf = dummy2(3, 0.01)
        G.build_edges_with_condition(condition_func=cf, use_cfunc_only=True)
        G.build_edges_with_condition(condition_func=cf, use_cfunc_only=False)

    def test_dfs(self):
        S1 = self.S1
        S2 = self.S2
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S2 = np.float64(np.matmul(S1[subset, :], rotmat) + [1, 1])
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()
        ans = G.get_correspondence(upper_bound=10, use_dfs=True, use_heuristic=False)

        subset.sort()
        assert ans[0] == subset

    def test_heuristic(self):
        S1 = self.S1
        S2 = self.S2
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S2 = np.float64(np.matmul(S1[subset, :], rotmat) + [1, 1])
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()
        ans = G.get_correspondence(upper_bound=10, use_dfs=False, use_heuristic=True)
        print(G)
        subset.sort()
        assert set(ans[0]) <= set(subset)

    def test_continue(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (100, 2)))
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:50]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S2 = np.float64(np.matmul(S1[subset, :], rotmat) + [1, 1])
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()
        while not G.search_done:
            ans = G.get_correspondence(
                upper_bound=100,
                time_limit=0.001,
                use_dfs=True,
                use_heuristic=False,
                continue_search=True,
            )
        ans = G.get_correspondence(
            upper_bound=100,
            time_limit=0.001,
            use_dfs=True,
            use_heuristic=False,
            continue_search=True,
        )
        subset.sort()
        assert set(ans[0]) == set(subset)

    def test_reset_search(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (100, 2)))
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:50]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S2 = np.float64(np.matmul(S1[subset, :], rotmat) + [1, 1])
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()

        with pytest.raises(RuntimeError):
            ans = G.get_correspondence(lower_bound=100, upper_bound=100)
        G.reset_search()

        ans = G.get_correspondence(lower_bound=19, upper_bound=100)
        subset.sort()
        assert set(ans[0]) == set(subset)

    def test_enumi(self):
        S1 = self.S1
        S2 = self.S2
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        rotmat = np.array(
            [
                [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
                [np.sin(np.pi / 3), np.cos(np.pi / 3)],
            ]
        )
        S2 = np.float64(np.matmul(S1[subset, :], rotmat) + [1, 1])
        G = cliquematch.A2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()
        ans = G.get_correspondence(upper_bound=10, use_dfs=True)
        subset.sort()
        assert set(ans[0]) == set(subset)

        c1 = list(x for x in G.all_correspondences(size=1))
        if len(c1) != G.n_vertices:
            warnings.warn("possible clique enumeration error", Warning)
        c9 = list(x for x in G.all_correspondences(size=len(ans[0]) - 1))
        if len(c9) != len(ans[0]):
            warnings.warn("possible clique enumeration error", Warning)

        c9b = list(
            x for x in G.all_correspondences(size=len(ans[0]) - 1, return_indices=False)
        )
        if len(c9) != len(c9b):
            warnings.warn("possible clique enumeration error", Warning)
        for i in range(len(c9)):
            t0 = self.S1[c9[i][0]]
            if not (c9b[i][0] == t0).all():
                warnings.warn("possible clique enumeration error", Warning)
