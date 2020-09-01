# -*- coding: utf-8 -*-
"""

    Molecule structure matching based on inter-atomic distances,
    using the algorithm described in:

    Gardiner, Eleanor J., Peter J. Artymiuk, and Peter Willett.
    "Clique-detection algorithms for matching three-dimensional molecular
    structures." Journal of Molecular Graphics and Modelling 15.4 (1997):
    245-253.

    The bzr_3d.sd dataset was obtained from:

    Sutherland, Jeffrey J., Lee A. O'brien, and Donald F. Weaver.
    "Spline-fitting with a genetic algorithm: A method for developing
    classification structure− activity relationships." Journal of chemical
    information and computer sciences 43.6 (2003): 1906-1915.

"""
import numpy as np
import cliquematch
from rdkit.Chem import PandasTools, Draw
from matplotlib import pyplot as plt


class MolData(object):
    def __init__(self, mol):
        self._mol = mol
        self._mol.Compute2DCoords()
        self.positions = np.array(self._mol.GetConformer().GetPositions())


def condition_closure(M1, M2):
    def condition(P, i1, j1, Q, i2, j2):
        b1 = M1.GetBondBetweenAtoms(i1, j1)
        b2 = M2.GetBondBetweenAtoms(i2, j2)
        if b1 is None and b2 is None:
            return True
        elif b1 is not None and b2 is not None:
            return ~(b1.IsInRing() ^ b2.IsInRing())
        else:
            return False

    return condition


def main():
    mols = PandasTools.LoadSDF("./bzr_3d.sd")
    data = [MolData(x) for x in mols["ROMol"]]
    plot(data[0], data[1], "comp_0_1.png")


def plot(d1, d2, filename=None):
    G = cliquematch.A2AGraph(d1.positions, d2.positions)
    cfunc = condition_closure(d1._mol, d2._mol)
    G.epsilon = 1e-6
    G.build_edges_with_condition(cfunc, False)
    sub1, sub2 = G.get_correspondence(
        use_heuristic=True, use_dfs=True, return_indices=True
    )

    fig = plt.figure(figsize=(12.1, 6.1))
    axs = fig.subplots(1, 2)
    mdraw = Draw.rdMolDraw2D.MolDraw2DCairo(600, 600)
    mdraw.drawOptions().fillHighlights = False

    Draw.rdMolDraw2D.PrepareAndDrawMolecule(
        mdraw,
        d1._mol,
        highlightAtoms=sub1,
        highlightAtomColors={
            int(x): (0.05 * i, 1.0 - 0.05 * i, 0.0) for i, x in enumerate(sub1)
        },
    )
    mdraw.WriteDrawingText("./mol1.png")
    img0 = plt.imread("./mol1.png")
    mdraw.ClearDrawing()
    axs[0].imshow(np.array(img0))
    axs[0].set_xticks([])
    axs[0].set_yticks([])

    Draw.rdMolDraw2D.PrepareAndDrawMolecule(
        mdraw,
        d2._mol,
        highlightAtoms=sub2,
        highlightAtomColors={
            int(x): (0.05 * i, 1.0 - 0.05 * i, 0.0) for i, x in enumerate(sub2)
        },
    )
    mdraw.WriteDrawingText("./mol2.png")
    mdraw.ClearDrawing()
    img1 = plt.imread("./mol2.png")
    axs[1].imshow(np.array(img1))
    axs[1].set_xticks([])
    axs[1].set_yticks([])

    fig.subplots_adjust(left=0.05, right=0.95, top=0.95, bottom=0.05)
    fig.savefig(filename)
    plt.close()
    print(
        len(d1.positions),
        len(d2.positions),
        G.n_edges,
        len(G.get_max_clique(use_heuristic=True, use_dfs=True)),
    )


if __name__ == "__main__":
    main()
