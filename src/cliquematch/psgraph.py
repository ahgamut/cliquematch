# -*- coding: utf-8 -*-
"""
    cliquematch.psgraph
    ~~~~~~~~~~~~~~~~~~~

    Wrapper over cliquematch.cm_base.A2AGraph 
    for aligning images using marked points

    :license: see LICENSE for more details.
"""

from .cm_base import A2AGraph as _A2AGraph
import numpy as np
from warnings import warn


class MaskFilter(object):
    """
    A class containing the required members to filter edges via 
    steve's rule for considering the validity of an alignment
    """

    def __init__(self, control_pts, img_mask, percentage=0.8):
        self._pts = np.copy(control_pts, order="F")
        self._mask = img_mask
        self.percentage = percentage
        self.tformed_pts = np.zeros(control_pts.shape, dtype=np.int64, order="F")
        self.valids = np.zeros(len(self._pts), dtype=np.bool)
        self._ctx = 0

    def __call__(self, mat1, i1, j1, mat2, i2, j2):
        # get the points
        s1 = mat1[[i1, j1], ::-1]  # ::-1 shifts from x,y to row,col
        s2 = mat2[[i2, j2], ::-1]

        # solve the linear equation
        # to get the required rotation and translation
        delta_s1 = s1[0] - s1[1]
        delta_s2 = s2[0] - s2[1]
        den = np.sum(delta_s1 ** 2)
        if den == 0:
            return False

        a = np.sum(delta_s1 * delta_s2) / den
        b = np.sum(delta_s2 * delta_s1[::-1] * [-1, 1]) / den
        c = np.array(
            [
                s2[0, 0] - a * s1[0, 0] + b * s1[0, 1],
                s2[0, 1] - b * s1[0, 0] - a * s1[0, 1],
            ]
        )

        # transform control points as per rotation and translation
        # this is slow because I have to convert floats to ints for indexing
        self.tformed_pts[:, 0] = self._pts[:, 0] * a - self._pts[:, 1] * b + c[0]
        self.tformed_pts[:, 1] = self._pts[:, 0] * b + self._pts[:, 1] * a + c[1]

        # check bounds
        self.valids = (
            (self.tformed_pts[:, 0] >= 0)
            & (self.tformed_pts[:, 1] >= 0)
            & (self.tformed_pts[:, 0] < self._mask.shape[0])
            & (self.tformed_pts[:, 1] < self._mask.shape[1])
        )

        self.tformed_pts[:, 0].clip(
            0, self._mask.shape[0] - 1, out=self.tformed_pts[:, 0]
        )
        self.tformed_pts[:, 1].clip(
            0, self._mask.shape[1] - 1, out=self.tformed_pts[:, 1]
        )

        # return true if percentage of control pts is enough
        msk_score = np.sum(
            self._mask[self.tformed_pts[:, 0], self.tformed_pts[:, 1]] & self.valids
        ) / len(self._pts)
        return msk_score >= self.percentage


class psgraph(_A2AGraph):
    """
    A convenience wrapper over `cliquematch.cm_base.A2AGraph`
    for the specific case of aligning corner points on an image
    """

    def __init__(
        self, set1, set2, d1=None, d2=None, is_d1_symmetric=True, is_d2_symmetric=True
    ):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        if d1 is not None:
            if d2 is not None:
                _A2AGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                    d2,
                    is_d2_symmetric,
                )
            else:
                warn("psgraph: Using default distance metric (Euclidean) for set S2")
                _A2AGraph.__init__(
                    self,
                    self.S1,
                    len(self.S1),
                    self.S2,
                    len(self.S2),
                    d1,
                    is_d1_symmetric,
                )
        else:
            warn("psgraph: Using default distance metric (Euclidean) for both arrays")
            _A2AGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _A2AGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _A2AGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def build_edges_via_steve_rule(self, control_pts, img_mask, percentage=0.8):
        mask_filter = MaskFilter(control_pts, img_mask, percentage)
        self.build_edges_with_condition(mask_filter, False)

    def get_correspondence(self):
        """
        Find correspondence between the sets of points `S1` and `S2`.

        Returns
        -------

        dict(ndarray, ndarray, float, float, float, ndarray)
            The two sets of corresponding points and the rotation/translation required
            to transform `S1` to `S2`
            (obtained via `Kabsch Algorithm <https://en.wikipedia.org/wiki/Kabsch_algorithm>`)
        """
        indices = _A2AGraph.get_correspondence(self)
        ans = [self.S1[indices[0], :], self.S2[indices[1], :]]
        # I want to find R, T such that S1*R + T = S2

        a11 = ans[0] - np.mean(ans[0], axis=0)
        a21 = ans[1] - np.mean(ans[1], axis=0)

        X = np.matmul(a11.T, a21)
        # TODO: Check for alternatives to below method
        V, s, U = np.linalg.svd(X, full_matrices=True, compute_uv=True)

        rotmat = np.eye(2, 2)

        d = np.linalg.det(np.matmul(V, U.T))
        rotmat[1, 1] = 1 if d > 0 else -1
        rotmat = np.matmul(np.matmul(V, rotmat), U.T)

        translation = -np.matmul(np.mean(ans[0], axis=0), rotmat) + np.mean(
            ans[1], axis=0
        )

        theta = np.arccos(rotmat[0, 0])
        theta = -theta if rotmat[1, 0] < 0 else theta
        theta = 0 if np.isnan(theta) else theta

        return {
            "S1": ans[0],
            "S2": ans[1],
            "theta": theta,
            "dx": translation[0],
            "dy": translation[1],
            "rotmat": rotmat,
        }
