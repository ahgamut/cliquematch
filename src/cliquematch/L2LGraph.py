# -*- coding: utf-8 -*-
"""
   cliquematch.L2LGraph
   ~~~~~~~~~~~~~~~~~~~~

   A convenience wrapper over cliquematch.cm_base.L2LGraph
 
   :copyright: (c) 2019 by gnv3.
   :license: see LICENSE for more details.
"""

from .cm_base import L2LGraph as _L2LGraph
import numpy as np
from warnings import warn


class L2LGraph(_L2LGraph):

    """Wrapper class adding functionality for keeping lists"""

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        """
        A simple wrapper over the base class, just to avoid copying
        """
        assert isinstance(set1, list)
        assert isinstance(set2, list)
        self.S1 = set1
        self.S2 = set2
        if d1 is not None:
            if d2 is not None:
                _L2LGraph.__init__(
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
                warn("L2LGraph: List S2 has no distance metric")
                _L2LGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                )
        else:
            warn("L2LGraph: Lists have no distance metric")
            _L2LGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _L2LGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _L2LGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.L2LGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[List, List]

        """
        indices = _L2LGraph.get_correspondence(self)
        if not return_indices:
            answer = [
                [self.S1[x] for x in indices[0]],
                [self.S2[x] for x in indices[1]],
            ]
        else:
            answer = indices

        return answer
