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
        inv_map = dict()
        v2_inv = lambda z: set(inv_map[x] for x in z)
        for i in range(len(indices[0])):
            mapping[indices[0][i]] = indices[1][i]
            inv_map[indices[1][i]] = indices[0][i]
        for i in range(len(indices[0])):
            G2[indices[1][i]] = self.S2._vertex_neighbors(indices[1][i]) & v2
            G1[indices[0][i]] = v2_inv(G2[indices[1][i]])
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
        """Obtain the corresponding vertices in the subgraph isomorphism.

        Args:
            lower_bound (`int`\): set a lower bound for the size
            upper_bound (`int`\): set an upper bound for the size
            time_limit (`float`\):
                set a time limit for the search: a nonpositive value
                implies there is no time limit (use in conjunction
                with ``continue_search``\ ).
            use_heuristic (`bool`\):
                if `True`\, use the heuristic-based search to obtain
                a large clique quickly. Good for obtaining an initial lower bound.
            use_dfs (`bool`\):
                if `True`\, use the depth-first to obtain the clique. default is `True`\.
            continue_search (`bool`\):
                set as `True` to continue a clique search interrupted by ``time_limit``\.
            return_indices (`bool`\):
                if `True` return the vertices of the corresponding subgraphs,
                else return `dict`\s for each corresponding subgraph and
                a `dict` mapping the vertices.

        Raises:
            RuntimeError: if called before edges are constructed
            RuntimeError: if search parameters are invalid / clique is not found
            RuntimeError: if obtained correspondence is invalid
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
        """Find all correspondences of a given size.

        Args:
            size (`int`\): size of the corresponding subsets.
            return_indices (`bool`\):
                if `True` return the vertices of the corresponding subgraphs,
                else return `dict`\s for each corresponding subgraph and
                a `dict` mapping the vertices.

        Returns:
            `~cliquematch._WrappedIterator`:
                a wrapped `~cliquematch.core.CorrespondenceIterator` object,
                which yields correspondences as per ``return_indices``.

        Raises:
            RuntimeError: if called before edges are constructed
        """
        offset = lambda l: [x + 1 for x in l]
        if return_indices:
            return WrappedIterator(
                Graph._all_correspondences(self, len(self.S1), len(self.S2), size),
                lambda x: [offset(x[0]), offset(x[1])],
            )
        else:
            return WrappedIterator(
                Graph._all_correspondences(self, len(self.S1), len(self.S2), size),
                lambda x: self._format_correspondence([offset(x[0]), offset(x[1])]),
            )
