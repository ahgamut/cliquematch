# -*- coding: utf-8 -*-
"""
    Example application of cliquematch, an implementation of the
    CCMM algorithm described in:

    San Segundo, Pablo, and Jorge Artieda. "A novel clique formulation for the
    visual feature matching problem." Applied Intelligence 43.2 (2015):
    325-342.

    Download the dinosaur dataset from here:
        https://www.robots.ox.ac.uk/~vgg/data/mview/
"""
import cv2
import numpy as np
import cliquematch
from matplotlib import pyplot as plt
from matplotlib.patches import Circle, ConnectionPatch


class ImageData(object):
    surf = cv2.xfeatures2d.SURF_create(2000)

    def __init__(self, path, flags=0):
        self._path = path
        self.data = cv2.imread(path)
        kps, desc = self.surf.detectAndCompute(self.data, None)
        self.pts = np.float64(cv2.KeyPoint.convert(kps))
        self.desc = desc

    def __str__(self):
        return "{}: {}, {} keypoints, {} descriptors".format(
            self._path, self.data.shape, len(self.pts), self.desc.shape[1]
        )

    def plot(self, axis):
        axis.set_yticks([])
        axis.set_xticks([])
        axis.imshow(self.data[:, :, [2, 1, 0]])
        return axis


def get_Kbest_indices(S1, S2, K):
    indices = np.zeros((len(S1), len(S2)), np.bool)
    for i in range(len(S1)):
        s1i = S1[i, :]
        s2mod = np.sum((S2 - s1i) ** 2, 1)
        indices[i, np.argpartition(s2mod, K)[:K]] = True
    return indices


def view_correspondence(img1, img2):
    ind = get_Kbest_indices(img1.desc, img2.desc, 3)

    def condition_func(P, i1, j1, Q, i2, j2):
        return ind[i1, i2] & ind[j1, j2]

    G = cliquematch.A2AGraph(img1.pts, img2.pts)
    G.epsilon = 50
    G.build_edges_with_condition(condition_func, False)
    corr = G.get_correspondence(
        lower_bound=1,
        upper_bound=50,
        use_dfs=True,
        use_heuristic=True,
        return_indices=False,
    )
    clique_size = len(corr[0])

    fig = plt.figure()
    axs = fig.subplots(1, 2)

    img1.plot(axs[0])
    img2.plot(axs[1])

    for i in range(clique_size):
        axs[0].add_artist(Circle(corr[0][i], color="red", radius=1.5))
        axs[1].add_artist(Circle(corr[1][i], color="red", radius=1.5))
        axs[1].add_artist(
            ConnectionPatch(
                xyA=corr[1][i],
                xyB=corr[0][i],
                coordsA="data",
                coordsB="data",
                axesA=axs[1],
                axesB=axs[0],
                color="red",
                alpha=0.4,
            )
        )

    plt.show()
    plt.close()


def main():
    img1 = ImageData("viff.000.ppm")
    print(img1)
    img2 = ImageData("viff.002.ppm")
    print(img2)
    view_correspondence(img1, img2)


if __name__ == "__main__":
    main()
