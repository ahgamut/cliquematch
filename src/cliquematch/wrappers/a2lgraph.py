# -*- coding: utf-8 -*-
from ._gen_graph import GenGraph
from warnings import warn
import numpy as np


class A2LGraph(GenGraph):
    """Correspondence Graph wrapper for array-to-list mappings.

    Any general object can be passed for `.S2`; the user is required to define
    how elements are accessed.

    Attributes:
        S1 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
        S2 ( `object` ):
        d1 ( `callable` (`numpy.ndarray`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S1`,
                            defaults to Euclidean metric if `None`
        d2 ( `callable` (`list`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S2`
        is_d1_symmetric ( `bool` ):
        is_d2_symmetric ( `bool` ):
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        super(A2LGraph, self).__init__(
            set1, set2, d1, d2, is_d1_symmetric, is_d2_symmetric
        )
        self.S1 = np.float64(set1)
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
                warn(
                    "A2LGraph: build_edges will throw error without"
                    " distance metric to compare elements of S2"
                )
        else:
            warn(
                "A2LGraph: Using default distance metric (Euclidean) for S1, and"
                "build_edges will throw error without distance metric to compare list elements"
            )

    def _format_correspondence(self, indices):
        return (self.S1[indices[0]], [self.S2[x] for x in indices[1]])
