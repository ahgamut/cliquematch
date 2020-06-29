# -*- coding: utf-8 -*-
"""
    cliquematch.tests.test_l2a
    ~~~~~~~~~~~~~~~~~~~~~~~~~~

    testcases for cliquematch.L2AGraph

    :license: see LICENSE for more details.
"""

import pytest
import numpy as np
import cliquematch
import random


def eucd(mat, i, j):
    if isinstance(mat, list):
        ans = ((mat[i][0] - mat[j][0]) ** 2 + (mat[i][1] - mat[j][1]) ** 2) ** 0.5
    else:
        ans = np.sqrt(np.sum((mat[i, :] - mat[j, :]) * (mat[i, :] - mat[j, :])))
    # print(mat[i], mat[j], ans)
    return ans


class dummy(object):
    def __init__(self, N):
        self.N = N

    def __call__(self, mat, i, j):
        if isinstance(mat, list):
            ans = (
                abs(mat[i][0] - mat[j][0]) ** self.N
                + abs(mat[i][1] - mat[j][1]) ** self.N
            ) ** (1 / self.N)
        else:
            ans = np.power(np.sum(np.abs(mat[i, :] - mat[j, :]) ** self.N), 1 / self.N)


class dummy2(object):
    def __init__(self, N, eps=0.0001):
        self.N = N
        self.eps = eps

    def __call__(self, mat1, i1, j1, mat2, i2, j2):
        z1 = np.array(mat1[i1], np.float64)
        z2 = np.array(mat1[j1], np.float64)
        z3 = np.array(mat2[i2], np.float64)
        z4 = np.array(mat2[j2], np.float64)
        ans1 = np.power(np.sum(np.abs(z1 - z2) ** self.N), 1 / self.N)
        ans2 = np.power(np.sum(np.abs(z3 - z4) ** self.N), 1 / self.N)
        ans = np.abs(ans1 - ans2)
        # print(i1, j1, i2, j2, ans <= self.eps)
        return ans < self.eps


class TestL2AGraph(object):
    """
    Test out properties of L2AGraph using random numpy arrays

    * Loading with different callables
    * testing that edges can be built, with or without a cfunc
    * testing data access, dfs, and heuristic (heuristic is buggy)
    * testing time limits with and continue search
    """

    def test_loading(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
        S2 = np.float64(np.random.uniform(0, 100, (20, 2)))

        S1 = S1.tolist()

        ds1 = dummy(2)
        ds2 = dummy(3)
        G3 = cliquematch.L2AGraph(
            S1, S2, d1=ds1, d2=ds2, is_d1_symmetric=True, is_d2_symmetric=True
        )
        G3 = cliquematch.L2AGraph(
            S1, S2, d1=ds1, d2=eucd, is_d1_symmetric=True, is_d2_symmetric=False
        )
        G3 = cliquematch.L2AGraph(
            S1, S2, d1=eucd, d2=ds2, is_d1_symmetric=False, is_d2_symmetric=True
        )
        with pytest.warns(UserWarning):
            G3 = cliquematch.L2AGraph(
                S1, S2, d1=None, d2=ds1, is_d1_symmetric=False, is_d2_symmetric=False
            )

    def test_edge_build(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        S2 = S1[subset, :]
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.00001
        G.build_edges()

        with pytest.warns(UserWarning):
            G1 = cliquematch.L2AGraph(S1, S2)
            with pytest.raises(RuntimeError):
                G1.build_edges()
            with pytest.raises(RuntimeError):
                G1.build_edges_with_condition(lambda s1, i, j, s2, i2, j2: 0, False)

        with pytest.warns(UserWarning):
            G2 = cliquematch.L2AGraph(S1, S2, d1=eucd)
            G2.build_edges()
            G2.build_edges_with_condition(lambda s1, i, j, s2, i2, j2: 0, False)

    def test_edge_custom(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
        subset = list(x for x in range(20))
        random.shuffle(subset)
        subset = subset[:10]
        S2 = S1[subset, :]
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        cf = dummy2(3, 0.01)
        G.build_edges_with_condition(condition_func=cf, use_cfunc_only=True)
        G.build_edges_with_condition(condition_func=cf, use_cfunc_only=False)

    def test_dfs(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
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
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.time_limit = 100
        G.use_heuristic = False
        G.use_dfs = True
        G.upper_bound = 10
        G.build_edges()
        ans = G.get_correspondence()

        subset.sort()
        assert ans[0] == subset

    def test_heuristic(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
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
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.time_limit = 100
        G.use_heuristic = True
        G.use_dfs = True
        G.upper_bound = 10
        G.build_edges()
        ans = G.get_correspondence()

        subset.sort()
        assert set(ans[0]) <= set(subset)

    def test_data(self):
        np.random.seed(824)
        S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
        S2 = np.float64(np.random.uniform(0, 100, (20, 2)))
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)

        G.epsilon = 0.1
        G.S2[0, 0] = 0
        G.S1[0][1] = 0
        G.use_dfs = True
        G.use_heuristic = False
        G.upper_bound = 100
        G.time_limit = 200
        full_inf = repr(G)
        temp_inf = str(G)
        read_onlys = [
            G.current_vertex,
            G.search_done,
            G.n_vertices,
            G.n_edges,
        ]

        with pytest.raises(AttributeError):
            G.current_vertex = 25

        with pytest.raises(AttributeError):
            G.search_done = False

        with pytest.raises(AttributeError):
            G.n_vertices = 22

        with pytest.raises(AttributeError):
            G.n_edges = 31

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
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.time_limit = 0.001
        # G.use_heuristic = True
        G.use_dfs = True
        G.upper_bound = 100
        G.build_edges()
        while not G.search_done:
            G.continue_search()
            ans = G.get_correspondence()
        ans = G.get_correspondence()

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
        S1 = S1.tolist()
        G = cliquematch.L2AGraph(S1, S2, eucd, eucd)
        G.epsilon = 0.001
        G.build_edges()
        G.time_limit = 100
        # G.use_heuristic = True
        G.use_dfs = True
        G.lower_bound = 100  # will cause error
        G.upper_bound = 100
        with pytest.raises(RuntimeError):
            ans = G.get_correspondence()
        G.reset_search()
        G.lower_bound = 19  # will work
        ans = G.get_correspondence()
        subset.sort()

        subset.sort()
        assert set(ans[0]) == set(subset)
