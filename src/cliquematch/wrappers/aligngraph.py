# -*- coding: utf-8 -*-
"""
    cliquematch.AlignGraph
    ~~~~~~~~~~~~~~~~~~~~~~

    a wrapper over cliquematchcliquematch.core.AlignGraph
    to maintain ndarrays

    :copyright: (c) 2020 by Gautham Venkatasubramanian.
    :license: see LICENSE for more details.
"""
from cliquematch.core import AlignGraph as _AlignGraph
import numpy as np


class MaskFilter(object):
    """
    A class containing the required members to filter edges via
    steve's rule for considering the validity of an alignment
    """

    def __init__(self, control_pts, img_mask, percentage=0.8):
        self._pts = np.copy(control_pts)
        self._mask = img_mask
        self.percentage = percentage
        self.tformed_pts = np.zeros(control_pts.shape, dtype=np.int64)
        self.valids = np.zeros(len(self._pts), dtype=np.bool)
        self._ctx = 0

    def __call__(self, mat1, i1, j1, mat2, i2, j2):
        # get the points
        s1 = mat2[[i2, j2], :]
        s2 = mat1[[i1, j1], :]

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

        bpts = self.tformed_pts[self.valids, :]

        # return true if percentage of control pts is enough
        msk_score = np.sum(self._mask[bpts[:, 0], bpts[:, 1]]) / len(self._pts)
        return msk_score >= self.percentage


class AlignGraph(_AlignGraph):

    """Wrapper class adding functionality for keeping ndarrays"""

    def __init__(self, set1, set2):
        """
        A simple wrapper over the base class, just to avoid copying the ndarrays
        """
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        _AlignGraph.__init__(self, self.S1, len(self.S1), self.S2, len(self.S2))

    def build_edges(self):
        _AlignGraph.build_edges(self, self.S1, self.S2)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        _AlignGraph.build_edges_with_condition(
            self, self.S1, self.S2, condition_func, use_cfunc_only
        )

    def build_edges_with_filter(self, control_points, filter_mask, percentage):
        _AlignGraph.build_edges_with_filter(
            self, self.S1, self.S2, control_points, filter_mask, percentage
        )

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
        indices = _AlignGraph.get_correspondence(self)
        ans = [self.S1[indices[0], :], self.S2[indices[1], :]]
        # I want to find R, T such that S1*R + T = S2

        a11 = ans[0] - np.mean(ans[0], axis=0)
        a21 = ans[1] - np.mean(ans[1], axis=0)

        X = np.matmul(a11.T, a21)
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
