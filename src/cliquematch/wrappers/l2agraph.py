# -*- coding: utf-8 -*-
from cliquematch.core import L2AGraph as _L2AGraph
from warnings import warn
import numpy as np


class L2AGraph(_L2AGraph):
    """Correspondence Graph wrapper for list-to-array mappings.

    Any general object can be passed for ``S1``; the user is required to define
    how elements are accessed.

    Attributes:
        S1 ( `object` ):
        S2 ( `numpy.array` ): array elements are converted to `numpy.float64`
        d1 ( `Callable` ):  distance metric for elements in ``S1``
        d2 ( `Callable` ):  distance metric for elements in ``S2``,
                            defaults to Euclidean metric if `None`
        is_d1_symmetric ( `bool` ):
        is_d2_symmetric ( `bool` ):
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        _L2AGraph.__init__(self)
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

    def build_edges(self):
        """Build edges of the correspondence graph using distance metrics.

        Checks ``d1`` and ``d2`` for defaults before passing to base class.
        """
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _L2AGraph._build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        """Build edges of the correspondence graph using a given condition function.

        Args:
            condition_func ( `callable` ): must take parameters corresponding
                            to ``S1``, `int`, `int`, ``S2``,
                            `int`, `int`, and return `bool`
            use_cfunc_only ( `bool` ): if `True`, the distance metrics will not
                            be used to filter out edges (slower)

        Returns:
            `True` if construction was successful
        """
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _L2AGraph._build_edges_condition_only(*args)
        else:
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _L2AGraph._build_edges(*args)

    def get_correspondence(self, return_indices=True):
        """Get corresponding subsets between the sets ``S1`` and ``S2``.

        Args:
            return_indices ( `bool` ): if `True` return the indices of the
                            corresponding elements, else return the elements
        """
        indices = _L2AGraph._get_correspondence(self, len(self.S1), len(self.S2))
        if not return_indices:
            answer = [[self.S1[x] for x in indices[1]], self.S2[indices[0], :]]
        else:
            answer = [indices[0], indices[1]]

        return answer
