# -*- coding: utf-8 -*-
from cliquematch.core import AlignGraph as _AlignGraph
import numpy as np


class MaskFilter(object):
    """Custom condition to filter invalid alignments in `.AlignGraph` .

    C++ implementation of this function is called by default by
    `.AlignGraph.build_edges_with_filter`.  Checks if each possible edge in the
    correspondence graph produces a mapping of points such that a high
    ``percentage`` of ``control points`` fall entirely within a ``mask`` of the
    target image.

    Attributes:
        pts ( `numpy.ndarray` ) : control points to use in every alignment test
        mask ( `numpy.ndarray` ): a boolean mask showing valid regions in the
                                target image
        percentage ( `float` ): an alignment is valid if the number of control
                                points that fall within the mask are greater
                                than this value
    """

    def __init__(self, control_pts, img_mask, percentage=0.8):
        self.pts = np.copy(control_pts)
        self.mask = img_mask
        self.percentage = percentage
        self._tformed_pts = np.zeros(control_pts.shape, dtype=np.uint)
        self._valids = np.zeros(len(self.pts), dtype=np.bool)
        self._ctx = 0
        self._query = np.zeros((4, 4), np.float32)
        self._rhs = np.zeros((4, 1), np.float32)
        self._answer = np.zeros((4, 1), np.float32)

    def __call__(self, mat1, i1, j1, mat2, i2, j2):
        # get the points
        s1 = mat2[[i2, j2], :]
        s2 = mat1[[i1, j1], :]

        # solve the linear equation
        # to get the required rotation and translation
        delta_s1 = s1[0] - s1[1]
        den = np.sum(delta_s1 ** 2)
        if den == 0:
            return False

        self._query[0, :] = [s1[0, 0], -s1[0, 1], 1, 0]
        self._query[1, :] = [s1[0, 1], s1[0, 0], 0, 1]
        self._query[2, :] = [s1[1, 0], -s1[1, 1], 1, 0]
        self._query[3, :] = [s1[1, 1], s1[1, 0], 0, 1]
        self._rhs[:, 0] = [s2[0, 0], s2[0, 1], s2[1, 0], s2[1, 1]]
        self._answer = np.linalg.solve(self._query, self._rhs)
        a = self._answer[0]
        b = self._answer[1]
        c = self._answer[2:]

        # transform control points as per rotation and translation
        # this is slow because I have to convert floats to ints for indexing
        self._tformed_pts[:, 0] = self.pts[:, 0] * a - self.pts[:, 1] * b + c[0]
        self._tformed_pts[:, 1] = self.pts[:, 0] * b + self.pts[:, 1] * a + c[1]

        # check bounds
        self._valids = (self._tformed_pts[:, 1] < self.mask.shape[0]) & (
            self._tformed_pts[:, 0] < self.mask.shape[1]
        )

        bpts = self._tformed_pts[self._valids, :]

        # return true if percentage of control pts is enough
        msk_score = 1.0 * np.sum(self.mask[bpts[:, 1], bpts[:, 0]])
        msk_score = msk_score / len(self.pts)
        return msk_score >= self.percentage


class AlignGraph(_AlignGraph):
    """Correspondence graph for aligning images using obtained interest points.

    Uses a mask-based filtering method as a conditon function during
    construction of the graph. Default Euclidean metrics are used as distance
    metrics.

    Attributes:
        S1 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
        S2 ( `numpy.ndarray` ): array elements are converted to `numpy.float64`
    """

    def __init__(self, set1, set2):
        self.S1 = np.float64(set1)
        self.S2 = np.float64(set2)
        _AlignGraph.__init__(self)

    def build_edges(self):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2)]
        return _AlignGraph._build_edges_metric_only(*args)

    def build_edges_with_condition(self, condition_func, use_cfunc_only):
        args = [self, self.S1, len(self.S1), self.S2, len(self.S2), condition_func]
        if use_cfunc_only:
            return _AlignGraph._build_edges_condition_only(*args)
        else:
            return _AlignGraph._build_edges(*args)

    def build_edges_with_filter(self, control_points, filter_mask, percentage):
        """Uses control points and a binary mask to filter out invalid mappings
        and construct a correspondence graph.

        Args:
            control_points ( `numpy.ndarray` ) :
                            control points to use in every alignment test
            filter_mask ( `numpy.ndarray` ):
                            a boolean mask showing valid regions in the
                            target image
            percentage ( `float` ):
                            an alignment is valid if the number of control
                            points that fall within the mask are greater
                            than this value
        """
        args = [
            self,
            self.S1,
            len(self.S1),
            self.S2,
            len(self.S2),
            control_points,
            filter_mask,
            percentage,
        ]
        _AlignGraph._build_edges_with_filter(*args)

    def get_correspondence(self):
        """Find correspondence between the sets of points ``S1`` and ``S2``.

        Returns
        -------

        `dict`
            The two sets of corresponding points and the rotation/translation required
            to transform `S1` to `S2`
            (obtained via `Kabsch Algorithm <https://en.wikipedia.org/wiki/Kabsch_algorithm>`)
        """
        indices = _AlignGraph._get_correspondence(self, len(self.S1), len(self.S2))
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
