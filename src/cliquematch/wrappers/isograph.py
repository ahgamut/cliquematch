# -*- coding: utf-8 -*-
"""
    cliquematch.IsoGraph
    ~~~~~~~~~~~~~~~~~~~~

    A wrapper class over cliquematchcliquematch.core.L2LGraph
    to find subgraph isomorphisms using cliques in a 
    product graph

    :copyright: (c) 2019 by gnv3.
    :license: see LICENSE for more details.
"""
from cliquematch.core import L2LGraph as _IsoGraph
from warnings import warn


edge_condition_function = lambda s, i, j: 1 if j in s[i] else 0


class IsoGraph(_IsoGraph):
    def __init__(self, set1, set2):
        """
        A simple wrapper over the base class, just to save the Graphs here
        """
        self.S1 = set1
        self.S2 = set2

        for x in self.S1:
            assert isinstance(x, set), "Adjacency List of Graph must be List[Set]"
        for x in self.S2:
            assert isinstance(x, set), "Adjacency List of Graph must be List[Set]"

        _IsoGraph.__init__(
            self,
            self.S1,
            len(self.S1),
            self.S2,
            len(self.S2),
            edge_condition_function,
            True,
            edge_condition_function,
            True,
        )

    def build_edges(self):
        _IsoGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _IsoGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def get_correspondence(self, return_indices=True):
        """
        Wrapper over cm_base.L2LGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding graph vertices
            else it returns the corresponding graphs themselves
        :returns: List

        """
        indices = _IsoGraph.get_correspondence(self)
        indices = [sorted(indices[0]), sorted(indices[1])]
        if not return_indices:
            answer = [
                [self.S1[x] for x in indices[0]],
                [self.S2[x] for x in indices[1]],
            ]
            for i in range(len(answer)):
                adjlist = answer[i]
                for st in adjlist:
                    st = st.intersection(set(indices[i]))
        else:
            answer = indices

        return answer
