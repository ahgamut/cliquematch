# -*- coding: utf-8 -*-
from cliquematch.core import A2LGraph as _A2LGraph
from warnings import warn
import numpy as np


class A2LGraph(_A2LGraph):
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
        _A2LGraph.__init__(self)
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

    def build_edges(self):
        """Build edges of the correspondence graph using distance metrics.

        Checks `.d1` and `.d2` for defaults before passing to base class.
        """
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _A2LGraph._build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        """Build edges of the correspondence graph using a given condition function.

        Args:
            condition_func ( `callable` ): must take parameters corresponding
                            to `.S1`, `int`, `int`, `.S2`,
                            `int`, `int`, and return `bool`
            use_cfunc_only ( `bool` ): if `True`, the distance metrics will not
                            be used to filter out edges (slower)

        Returns:
            `True` if construction was successful
        """
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _A2LGraph._build_edges_condition_only(*args)
        else:
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _A2LGraph._build_edges(*args)

    def get_correspondence(self, return_indices=True):
        """Get corresponding subsets between the sets `.S1` and `.S2`.

        Args:
            return_indices ( `bool` ): if `True` return the indices of the
                            corresponding elements, else return the elements
        """
        indices = _A2LGraph._get_correspondence(self, len(self.S1), len(self.S2))
        if not return_indices:
            answer = [self.S1[indices[0], :], [self.S2[x] for x in indices[1]]]
        else:
            answer = [indices[0], indices[1]]

        return answer
