# -*- coding: utf-8 -*-
from ._gen_graph import GenGraph
from warnings import warn
import numpy as np


class L2AGraph(GenGraph):
    """Correspondence Graph wrapper for list-to-array mappings.

    Any general object can be passed for `.S1`; the user is required to define
    how elements are accessed.

    Attributes:
        S1 ( `object` ):
        S2 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
        d1 ( `callable` (`list`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S1`
        d2 ( `callable` (`numpy.ndarray`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S2`,
                            defaults to Euclidean metric if `None`
        is_d1_symmetric ( `bool` ):
        is_d2_symmetric ( `bool` ):
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        super(L2AGraph, self).__init__(
            set1, set2, d1, d2, is_d1_symmetric, is_d2_symmetric
        )
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

    def _format_correspondence(self, indices):
        return ([self.S1[x] for x in indices[0]], self.S2[indices[1]])
