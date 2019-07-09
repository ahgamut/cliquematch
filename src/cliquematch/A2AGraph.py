# -*- coding: utf-8 -*-
"""
   cliquematch.A2AGraph
   ~~~~~~~~~~~~~~~~~~~~

   A convenience wrapper over cliquematch.cm_base.A2AGraph
 
   :copyright: (c) 2019 by gnv3.
   :license: see LICENSE for more details.
"""

from .cm_base import A2AGraph as _A2AGraph
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
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        if d1 is not None:
            if d2 is not None:
                _A2AGraph.__init__(
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
                warn("A2AGraph: Using default distance metric (Euclidean) for set S2")
                _A2AGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                )
        else:
            warn("A2AGraph: Using default distance metric (Euclidean) for both arrays")
            _A2AGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _A2AGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _A2AGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.A2AGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[ndarray, ndarray]

        """
        indices = _A2AGraph.get_correspondence(self)
        if not return_indices:
            answer = [self.S1[indices[0], :], self.S2[indices[1], :]]
        else:
            answer = [np.array(indices[0], np.uint32), np.array(indices[1], np.uint32)]

        return answer
