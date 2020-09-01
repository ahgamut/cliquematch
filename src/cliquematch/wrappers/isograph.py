# -*- coding: utf-8 -*-
from cliquematch.core import Graph, _build_edges
from warnings import warn


class IsoGraph(Graph):
    """Correspondence graph for finding subgraph isomorphisms.

    Attributes:
        S1 ( `cliquematch.Graph` ):
        S2 ( `cliquematch.Graph` ):
    """

    def __init__(self, G1, G2):
        super(IsoGraph, self).__init__()
        self.S1 = G1
        self.S2 = G2

    def build_edges(self):
        """Build edges of the correspondence graph.
        """
        _build_edges(self, self.S1, self.S2)

    def _format_correspondence(self, indices):
        v1 = set(indices[0])
        v2 = set(indices[0])
        G1 = dict()
        G2 = dict()
        mapping = dict()
        inv_map = dict()
        v2_inv = lambda z: set(inv_map[x] for x in z)
        for i in range(len(indices[0])):
            mapping[indices[0][i]] = indices[1][i]
            inv_map[indices[1][i]] = indices[0][i]
        for i in range(len(indices[0])):
            G2[indices[1][i]] = self.S2._vertex_neighbors(indices[1][i]) & v2
            G1[indices[0][i]] = v2_inv(G2[indices[1][i]])
        return (G1, G2, mapping)

    def get_correspondence(self, return_indices=True):
        """Obtain corresponding vertices in the subgraph isomorphism.

        Args:
            return_indices(`bool`\ ):
                if `False` will the return a `tuple` of 3 `dict` s:
                the subgraph of `.S1`, the subgraph `.S2`, a mapping
                from the vertices of `.S1` to `.S1`

        Returns:
            `list` or `tuple` (see above).
        """
        indices = self._get_correspondence(self.S1.n_vertices, self.S2.n_vertices)
        offset = lambda l: [x + 1 for x in l]
        indices = [offset(indices[0]), offset(indices[1])]
        if return_indices:
            return indices
        else:
            return self._format_correspondence(indices)
