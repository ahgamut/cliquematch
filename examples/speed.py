# -*- coding: utf-8 -*-
import cliquematch
import argparse
import time

BENCHMARK_GRAPHS = {
    "ca-AstroPh": ("", False),
    "ca-CondMat": ("", False),
    "ca-GrQc": ("", False),
    "ca-HepPh": ("", False),
    "ca-HepTh": ("", False),
    "Erdos02": ("", False),
    "Erdos972": ("", False),
    "fe_body": ("", False),
    "fe_sphere": ("", False),
    "cti": ("", False),
    "delaunay_n13": ("", False),
    "cond-mat-2003": ("", True),
}
HELP_STRING = (
    "Check speed of cliquematch for various benchmark graph(s):\n\n\t"
    + "\n\t".join(["%s \t %s" % (k, v[0]) for k, v in BENCHMARK_GRAPHS.items()])
)


def runner(name_list, use_dfs, use_heuristic):
    print(
        "{:<15s}{:>15s}{:>15s}{:>15s}{:>15s}".format(
            "Name", "#Vertices", "#Edges", "Clique Size", "Time"
        )
    )
    for x in name_list:
        G = cliquematch.Graph.from_file(x + ".mtx")
        G.upper_bound = 1000
        G.time_limit = 100
        G.use_dfs = use_dfs
        G.use_heuristic = use_heuristic
        start = time.time()
        ans = G.get_max_clique()
        t = time.time() - start
        print(
            "{:<15s}{:>15d}{:>15d}{:>15d}{:>15.5f}".format(
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
    runner(a.names, bool(a.use_dfs), bool(a.use_heuristic))


if __name__ == "__main__":
    main()
