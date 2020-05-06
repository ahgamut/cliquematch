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
from cliquematch.core import IsoGraph as _IsoGraph
from warnings import warn


class IsoGraph(_IsoGraph):
    def __init__(self, G1, G2):
        """
        A simple wrapper over the base class, just to save the Graphs here
        """
        _IsoGraph.__init__(self)
        self.S1 = G1
        self.S2 = G2

    def build_edges(self):
        _IsoGraph.build_edges(self, self.S1, self.S2)

    def get_correspondence(self):
        """
        Wrapper over cm_base.L2LGraph.get_correspondence

        :return_indices: bool
            if true, returns the indices of the corresponding graph vertices
            else it returns the corresponding graphs themselves
        :returns: List

        """
        indices = _IsoGraph.get_correspondence(
            self, self.S1.n_vertices, self.S2.n_vertices
        )
        offset = lambda l: [x + 1 for x in l]
        answer = [offset(indices[0]), offset(indices[1])]
        return answer
