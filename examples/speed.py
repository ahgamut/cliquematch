# -*- coding: utf-8 -*-
import cliquematch
import argparse
import time

BENCHMARK_GRAPHS = {
    "Erdos02": "https://sparse.tamu.edu/Pajek/Erdos02",
    "Erdos972": "https://sparse.tamu.edu/Pajek/Erdos972",
    "Erdos982": "https://sparse.tamu.edu/Pajek/Erdos982",
    "Erdos992": "https://sparse.tamu.edu/Pajek/Erdos992",
    "ca-AstroPh": "https://sparse.tamu.edu/SNAP/ca-AstroPh",
    "ca-CondMat": "https://sparse.tamu.edu/SNAP/ca-CondMat",
    "ca-GrQc": "https://sparse.tamu.edu/SNAP/ca-GrQc",
    "ca-HepPh": "https://sparse.tamu.edu/SNAP/ca-HepPh",
    "ca-HepTh": "https://sparse.tamu.edu/SNAP/ca-HepTh",
    "caidaRouterLevel": "https://sparse.tamu.edu/DIMACS10/caidaRouterLevel",
    "coPapersCiteseer": "https://sparse.tamu.edu/DIMACS10/coPapersCiteseer",
    "cond-mat-2003": "https://sparse.tamu.edu/Newman/cond-mat-2003",
    "cti": "https://sparse.tamu.edu/DIMACS10/cti",
}
HELP_STRING = (
    "Check speed of cliquematch for various benchmark graph(s).\n"
    "It requires .mtx files downloaded and extracted from the following links:\n\n\t"
    + "\n\t".join(["%s \t %s" % (k, v) for k, v in BENCHMARK_GRAPHS.items()])
)


def runner(name_list, use_dfs, use_heuristic):
    print(
        "{:<18s}{:>18s}{:>18s}{:>18s}{:>18s}".format(
            "Name", "#Vertices", "#Edges", "Clique Size", "Time"
        )
    )
    for x in name_list:
        G = cliquematch.Graph.from_file(x + ".mtx")
        start = time.time()
        ans = G.get_max_clique(
            lower_bound=1,
            upper_bound=65535,
            use_dfs=use_dfs,
            use_heuristic=use_heuristic,
        )
        t = time.time() - start
        print(
            "{:<18s}{:>18d}{:>18d}{:>18d}{:>18.5f}".format(
                x, G.n_vertices, G.n_edges, len(ans), t
            )
        )


def main():
    parser = argparse.ArgumentParser(
        description=HELP_STRING, formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("names", nargs="*", help="Name(s) of benchmark graphs")
    parser.add_argument(
        "-u",
        "--use-heuristic",
        default=False,
        type=int,
        help="Use the heuristic search method (0 or 1)",
    )
    parser.add_argument(
        "-d",
        "--use-dfs",
        default=True,
        type=int,
        help="Use the depth-first search method (0 or 1)",
    )
    a = parser.parse_args()
    if a.names == []:
        a.names = sorted(list(BENCHMARK_GRAPHS.keys()))
    else:
        for x in a.names:
            assert (
                x in BENCHMARK_GRAPHS.keys()
            ), "%s is invalid benchmark graph name" % (x)

    runner(a.names, bool(a.use_dfs), bool(a.use_heuristic))


if __name__ == "__main__":
    main()
