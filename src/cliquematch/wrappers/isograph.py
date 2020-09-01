# -*- coding: utf-8 -*-
from cliquematch.core import Graph, _build_edges
from ._gen_graph import WrappedIterator
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
        for i in range(len(indices[0])):
            G1[indices[0][i]] = self.S1._vertex_neighbors(indices[0][i]) & v1
            G2[indices[1][i]] = self.S2._vertex_neighbors(indices[1][i]) & v2
            mapping[indices[0][i]] = indices[1][i]
        return (G1, G2, mapping)

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
        """Obtain indices of the corresponding vertices in the subgraph isomorphism.
        """
        indices = self._get_correspondence(
            self.S1.n_vertices,
            self.S2.n_vertices,
            lower_bound,
            upper_bound,
            time_limit,
            use_heuristic,
            use_dfs,
            continue_search,
        )
        offset = lambda l: [x + 1 for x in l]
        indices = [offset(indices[0]), offset(indices[1])]
        if return_indices:
            return indices
        else:
            return self._format_correspondence(indices)

    def all_correspondences(self, size, return_indices=True):
        if return_indices:
            return Graph._all_correspondences(self, len(self.S1), len(self.S2), size)
        else:
            return WrappedIterator(
                Graph._all_correspondences(self, len(self.S1), len(self.S2), size),
                lambda x: self._format_correspondence(x),
            )
