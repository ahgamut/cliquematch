# -*- coding: utf-8 -*-
from cliquematch.core import IsoGraph as _IsoGraph
from warnings import warn


class IsoGraph(_IsoGraph):
    """Correspondence graph for finding subgraph isomorphisms.

    Attributes:
        S1 ( `cliquematch.Graph` ):
        S2 ( `cliquematch.Graph` ):
    """

    def __init__(self, G1, G2):
        _IsoGraph.__init__(self)
        self.S1 = G1
        self.S2 = G2

    def build_edges(self):
        """Build edges of the correspondence graph.
        """
        _IsoGraph._build_edges(self, self.S1, self.S2)

    def get_correspondence(self):
        """Obtain indices of the corresponding vertices in the subgraph isomorphism.
        """
        indices = _IsoGraph._get_correspondence(
            self, self.S1.n_vertices, self.S2.n_vertices
        )
        offset = lambda l: [x + 1 for x in l]
        answer = [offset(indices[0]), offset(indices[1])]
        return answer
