# -*- coding: utf-8 -*-
from ._gen_graph import GenGraph
from warnings import warn


class L2LGraph(GenGraph):
    """Correspondence Graph wrapper for list-to-list mappings.

    Any `object` can be passed for `.S1` and `.S2`; the user is required to
    define how the elements are accessed.

    Attributes:
        S1 ( `object` ): array elements are converted to `numpy.float64`
        S2 ( `object` ): array elements are converted to `numpy.float64`
        d1 ( `callable` (`list`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S1`
        d2 ( `callable` (`list`, `int`, `int`) -> `float`):
                            distance metric for elements in `.S2`
        is_d1_symmetric ( `bool` ):
        is_d2_symmetric ( `bool` ):
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        super(L2LGraph, self).__init__(
            set1, set2, d1, d2, is_d1_symmetric, is_d2_symmetric
        )
        self.S1 = set1
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
                    "L2LGraph: build_edges with throw error without distance metric for elements of S1"
                )
        else:
            warn(
                "L2LGraph: build_edges with throw error without distance metric for S1 or S2"
            )

    def _format_correspondence(self, indices):
        return ([self.S1[x] for x in indices[0]], [self.S2[x] for x in indices[1]])
