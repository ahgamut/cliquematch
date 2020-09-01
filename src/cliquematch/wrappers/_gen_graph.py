# -*- coding: utf-8 -*-
from cliquematch.core import (
    Graph,
    _build_edges_metric_only,
    _build_edges,
    _build_edges_condition_only,
)


class WrappedIterator(object):
    def __init__(self, it, wrapper):
        self._it = it
        self._wrapper = wrapper

    def __iter__(self):
        return self

    def __next__(self):
        return self._wrapper(next(self._it))

    def next(self):
        return self.__next__()


class GenGraph(Graph):
    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        super(GenGraph, self).__init__()
        self.S1 = set1
        self.S2 = set2
        self.d1 = None
        self.d2 = None
        self.is_d1_symmetric = is_d1_symmetric
        self.is_d2_symmetric = is_d2_symmetric
        self.epsilon = 1.1

    def build_edges(self):
        """Build edges of the correspondence graph using distance metrics.

        Checks `.d1` and `.d2` for defaults before passing to base class.
        """
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), self.epsilon]
        if self.d1:
            args = args + [self.d1, self.is_d1_symmetric]
            if self.d2:
                args = args + [self.d2, self.is_d2_symmetric]
        return _build_edges_metric_only(*args)

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
        args = [
            self,
            self.S1,
            len(self.S1),
            self.S2,
            len(self.S2),
        ]
        if use_cfunc_only:
            args = args + [condition_func]
            return _build_edges_condition_only(*args)
        else:
            args = args + [self.epsilon, condition_func]
            if self.d1:
                args = args + [self.d1, self.is_d1_symmetric]
                if self.d2:
                    args = args + [self.d2, self.is_d2_symmetric]
            return _build_edges(*args)

    def _format_correspondence(self, indices):
        return indices

    def get_correspondence(
        self,
        lower_bound=1,
        upper_bound=0xFFFF,
        time_limit=-1.0,
        use_heuristic=True,
        use_dfs=True,
        continue_search=False,
        return_indices=True,
    ):
        """Get corresponding subsets between the `.S1` and `.S2`.

        Args:
            return_indices ( `bool` ): if `True` return the indices of the
                            corresponding elements, else return the elements
        """
        indices = Graph._get_correspondence(
            self,
            len(self.S1),
            len(self.S2),
            lower_bound,
            upper_bound,
            time_limit,
            use_heuristic,
            use_dfs,
            continue_search,
        )
        if return_indices:
            answer = indices
        else:
            answer = self._format_correspondence(indices)
        return answer

    def all_correspondences(self, size, return_indices=True):
        if return_indices:
            return Graph._all_correspondences(self, len(self.S1), len(self.S2), size)
        else:
            return WrappedIterator(
                Graph._all_correspondences(self, len(self.S1), len(self.S2), size),
                lambda x: self._format_correspondence(x),
            )
