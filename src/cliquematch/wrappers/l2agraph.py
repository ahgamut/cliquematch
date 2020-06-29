# -*- coding: utf-8 -*-
"""
    cliquematch.L2AGraph
    ~~~~~~~~~~~~~~~~~~~~

    A convenience wrapper over cliquematch.core.L2AGraph

    :license: see LICENSE for more details.
"""
from cliquematch.core import L2AGraph as _L2AGraph
import numpy as np
from warnings import warn


class L2AGraph(_L2AGraph):

    """Wrapper class adding functionality for keeping sets S1, S2"""

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        _L2AGraph.__init__(self)
        self.S1 = set1
        self.S2 = np.float64(set2)
        self.d1 = None
        self.d2 = None
        self.is_d1_symmetric = is_d1_symmetric
        self.is_d2_symmetric = is_d2_symmetric
        if d1 is not None:
            self.d1 = d1
            if d2 is not None:
                self.d2 = d2
            else:
                warn("L2AGraph: Using default distance metric (Euclidean) for set S2")
        else:
            warn(
                "L2AGraph: build_edges with throw error without distance metric for elements of S1,"
                "Using default distance metric (Euclidean) for set S2"
            )

    def build_edges(self):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _L2AGraph._build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _L2AGraph._build_edges_condition_only(*args)
        else:
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _L2AGraph._build_edges(*args)

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over core.L2AGraph._get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[List, List]

        """
        indices = _L2AGraph._get_correspondence(self, len(self.S1), len(self.S2))
        if not return_indices:
            answer = [[self.S1[x] for x in indices[1]], self.S2[indices[0], :]]
        else:
            answer = [indices[0], indices[1]]

        return answer
