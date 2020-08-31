# -*- coding: utf-8 -*-
from ._gen_graph import GenGraph
from warnings import warn
import numpy as np


class A2AGraph(GenGraph):
    """Correspondence Graph wrapper for array-to-array mappings.

    Attributes:
        S1 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
        S2 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
        d1 ( `callable` (`numpy.ndarray`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S1`,
                            defaults to Euclidean metric if `None`.
        d2 ( `callable` (`numpy.ndarray`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S2`,
                            defaults to Euclidean metric if `None`
        is_d1_symmetric ( `bool` ):
        is_d2_symmetric ( `bool` ):
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        super(A2AGraph, self).__init__(
            set1, set2, d1, d2, is_d1_symmetric, is_d2_symmetric
        )
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        if d1 is not None:
            self.d1 = d1
            if d2 is not None:
                self.d2 = d2
            else:
                warn("A2AGraph: Using default distance metric (Euclidean) for set S2")
        else:
            warn("A2AGraph: Using default distance metric (Euclidean) for both arrays")

    def _format_correspondence(self, indices):
        return (self.S1[indices[0]], self.S2[indices[1]])
