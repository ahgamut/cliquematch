# -*- coding: utf-8 -*-
"""
    cliquematch.L2AGraph
    ~~~~~~~~~~~~~~~~~~~~

    A convenience wrapper over cliquematchcliquematch.core.L2AGraph

    :copyright: (c) 2019 by gnv3.
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
        assert isinstance(set1, list)
        self.S1 = set1
        self.S2 = np.float64(set2)
        if d1 is not None:
            if d2 is not None:
                _L2AGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                    d2,
                    is_d2_symmetric,
                )
            else:
                warn("L2AGraph: Using default distance metric (Euclidean) for array S2")
                _L2AGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                )
        else:
            warn(
                "L2AGraph: List S1 has no distance metric, Using default distance metric (Euclidean) for array S2"
            )
            _L2AGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _L2AGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _L2AGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.L2AGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[List, List]

        """
        indices = _L2AGraph.get_correspondence(self)
        if not return_indices:
            answer = [[self.S1[x] for x in indices[1]], self.S2[indices[0], :]]
        else:
            answer = [indices[0], indices[1]]

        return answer
