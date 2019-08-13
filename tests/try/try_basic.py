"""
NIST Forensic Footwear Team
Project Folder: cliquematch
Filename: test_cm_base.py
Author: gnv3 (Gautham Venkatasubramanian)
Created on: 11/26/18 9:14 AM

test_cm_base.py used for:

Testing usage of cliquematch with graph and points
"""

import numpy as np
import ctypes
import cliquematch
import sys, os


def test_small():
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
        ]
    )
    G = cliquematch.Graph(edges, 8)
    G.use_dfs = True
    G.upper_bound = 100
    G.time_limit = 200
    print(repr(G))
    print(str(G))
    ans = G.get_max_clique()
    print(ans)


def test_graph():

    mtxpth = os.path.abspath("../cond-mat-2003.mtx")
    G = cliquematch.Graph(mtxpth, 2)

    # G.use_heuristic = True
    G.use_dfs = True
    G.upper_bound = 100
    G.time_limit = 0.05

    print(G)
    ans = None

    while not G.search_done:
        print("Searched until: %d" % (G.current_vertex))
        G.continue_search()
        ans = G.get_max_clique()
        print(ans)
        print("\n\n")

    print(ans)


def test_pointset():
    S1 = np.float64(np.random.uniform(0, 100, (20, 2)))
    subset = np.random.choice(list(x for x in range(1, 20)), 10, replace=False)
    subset[0] = 0

    S1_sub = S1[subset, :]

    rotmat = np.array(
        [
            [np.cos(np.pi / 3), -np.sin(np.pi / 3)],
            [np.sin(np.pi / 3), np.cos(np.pi / 3)],
        ]
    )
    S2 = np.float64(np.matmul(S1_sub, rotmat) + [1, 1])

    G = cliquematch.psgraph(S1, S2)
    G.epsilon = 0.00001
    z = np.ones((100, 100), dtype=np.bool)
    G.build_edges()

    G.use_dfs = True
    G.upper_bound = 100
    G.time_limit = 10

    ans = None
    print(S1, "\n", S2)

    print(
        "S1 within numpy: ",
        S1[0, 0],
        S1.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
    )
    print(
        "S2 within numpy: ",
        S2[0, 0],
        S2.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
    )
    print(G)

    while not G.search_done:
        print("Searched until: %d" % (G.current_vertex))
        G.continue_search()
        ans = G.get_correspondence()
        print("Current clique size = %d" % (len(ans["S1"])))
        print("\n\n")

    # print(ans)
    print(G.__dict__, vars(G))
    # print(rotmat-ans["rotmat"])
    print("To transform S1 into S2, the rigid transformation is: ")
    print(
        "Theta = %.02f, dx = %.02f, dy = %.02f"
        % (180 * ans["theta"] / np.pi, ans["dx"], ans["dy"])
    )


if __name__ == "__main__":
    test_small()
    test_graph()
    test_pointset()
