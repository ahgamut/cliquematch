# -*- coding: utf-8 -*-
"""
    cliquematch.A2LGraph
    ~~~~~~~~~~~~~~~~~~~~

    A convenience wrapper over cliquematchcliquematch.core.A2LGraph

    :copyright: (c) 2019 by gnv3.
    :license: see LICENSE for more details.
"""
from cliquematch.core import A2LGraph as _A2LGraph
import numpy as np
from warnings import warn


class A2LGraph(_A2LGraph):

    """Wrapper class adding functionality for keeping sets S1, S2"""

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        _A2LGraph.__init__(self)
        self.S1 = np.float64(set1)
        assert isinstance(set2, list)
        self.S2 = set2
        self.d1 = None
        self.d2 = None
        self.is_d1_symmetric = is_d1_symmetric
        self.is_d2_symmetric = is_d2_symmetric
        if d1 is not None:
            self.d1 = d1
            if d2 is not None:
                self.d2 = d2
            else:
                warn("A2LGraph: Using default distance metric (Euclidean) for set S2")
        else:
            warn("A2LGraph: Using default distance metric (Euclidean) for both sets")

    def build_edges(self):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _A2LGraph.build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _A2LGraph.build_edges_condition_only(*args)
        else:
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _A2LGraph.build_edges(*args)

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.A2LGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[ndarray, ndarray]

        """
        indices = _A2LGraph.get_correspondence(self, len(self.S1), len(self.S2))
        if not return_indices:
            answer = [self.S1[indices[0], :], [self.S2[x] for x in indices[1]]]
        else:
            answer = [indices[0], indices[1]]

        return answer
