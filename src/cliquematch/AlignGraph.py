# -*- coding: utf-8 -*-
"""
    cliquematch.AlignGraph
    ~~~~~~~~~~~~~~~~~~~~~~

    a wrapper over cliquematch.core.AlignGraph
    to maintain ndarrays

    :copyright: (c) 2020 by Gautham Venkatasubramanian.
    :license: see LICENSE for more details.
"""
from .core import AlignGraph as _AlignGraph
import numpy as np


class AlignGraph(_AlignGraph):

    """Wrapper class adding functionality for keeping ndarrays"""

    def __init__(self, set1, set2):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        _AlignGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _AlignGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _AlignGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def build_edges_with_filter(self, control_points, filter_mask, percentage):
        _AlignGraph.build_edges_with_filter(
            self, self.S1, self.S2, control_points, filter_mask, percentage
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.AlignGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding points
            else it returns the subsets of points themselves
        :returns: List[ndarray, ndarray]

        """
        indices = _AlignGraph.get_correspondence(self)
        if not return_indices:
            answer = [self.S1[indices[0], :], self.S2[indices[1], :]]
        else:
            answer = [np.array(indices[0], np.uint32), np.array(indices[1], np.uint32)]

        return answer
