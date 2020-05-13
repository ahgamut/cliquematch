# -*- coding: utf-8 -*-
"""
   cliquematch.A2AGraph
   ~~~~~~~~~~~~~~~~~~~~

   A convenience wrapper over cliquematch.core.A2AGraph

   :copyright: (c) 2019 by gnv3.
   :license: see LICENSE for more details.
"""

from cliquematch.core import A2AGraph as _A2AGraph
import numpy as np
from warnings import warn


class A2AGraph(_A2AGraph):

    """Wrapper class adding functionality for keeping ndarrays"""

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        _A2AGraph.__init__(self)
        self.S1 = np.float64(set1)
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
                warn("A2AGraph: Using default distance metric (Euclidean) for set S2")
        else:
            warn("A2AGraph: Using default distance metric (Euclidean) for both arrays")

    def build_edges(self):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _A2AGraph._build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _A2AGraph._build_edges_condition_only(*args)
        else:
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _A2AGraph._build_edges(*args)

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over core.A2AGraph._get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[ndarray, ndarray]

        """
        indices = _A2AGraph._get_correspondence(self, len(self.S1), len(self.S2))
        if not return_indices:
            answer = [self.S1[indices[0], :], self.S2[indices[1], :]]
        else:
            answer = [np.array(indices[0], np.uint32), np.array(indices[1], np.uint32)]

        return answer
