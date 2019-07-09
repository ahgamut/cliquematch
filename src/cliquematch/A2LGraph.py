# -*- coding: utf-8 -*-
"""
    cliquematch.A2LGraph
    ~~~~~~~~~~~~~~~~~~~~

    A convenience wrapper over cliquematch.cm_base.A2LGraph

    :copyright: (c) 2019 by gnv3.
    :license: see LICENSE for more details.
"""
from .cm_base import A2LGraph as _A2LGraph
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
        self.S1 = np.float64(set1)
        assert isinstance(set2, list)
        self.S2 = set2
        if d1 is not None:
            if d2 is not None:
                _A2LGraph.__init__(
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
                warn("A2LGraph: List S2 has no distance metric")
                _A2LGraph.__init__(
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
                "A2LGraph: List S2 has no distance metric, Using default distance metric (Euclidean) for array S1"
            )
            _A2LGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _A2LGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _A2LGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.A2LGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[ndarray, ndarray]

        """
        indices = _A2LGraph.get_correspondence(self)
        if not return_indices:
            answer = [self.S1[indices[0], :], [self.S2[x] for x in indices[1]]]
        else:
            answer = [indices[0], indices[1]]

        return answer
