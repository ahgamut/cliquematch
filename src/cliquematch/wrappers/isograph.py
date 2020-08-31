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
