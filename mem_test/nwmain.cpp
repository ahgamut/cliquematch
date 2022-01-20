#include <detail/nwgraph/nwgraph.h>
#include <detail/nwgraph/nwclique.h>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

namespace cmd = cliquematch::detail;

void setup(std::pair<std::vector<u64>, std::vector<u64>>& edges,
           std::vector<double>& weights)
{
    u64 nvert = 5;
    u64 nedges = 6;

    edges.first.reserve(nvert + 1 + (2 * nedges));
    edges.second.reserve(nvert + 1 + (2 * nedges));
    weights.reserve(nvert + 1);

    edges.first.push_back(1);
    edges.second.push_back(2);
    edges.first.push_back(2);
    edges.second.push_back(1);

    edges.first.push_back(1);
    edges.second.push_back(3);
    edges.first.push_back(3);
    edges.second.push_back(1);

    edges.first.push_back(1);
    edges.second.push_back(4);
    edges.first.push_back(4);
    edges.second.push_back(1);

    edges.first.push_back(1);
    edges.second.push_back(5);
    edges.first.push_back(5);
    edges.second.push_back(1);

    edges.first.push_back(2);
    edges.second.push_back(4);
    edges.first.push_back(4);
    edges.second.push_back(2);

    edges.first.push_back(3);
    edges.second.push_back(5);
    edges.first.push_back(5);
    edges.second.push_back(3);

    weights.push_back(0);
    edges.first.push_back(0);
    edges.second.push_back(0);
    for (u64 i = 1; i <= nvert; i++)
    {
        weights.push_back(1.1);
        edges.first.push_back(i);
        edges.second.push_back(i);
    }
}

int main()
{
    std::pair<std::vector<u64>, std::vector<u64>> e;
    std::vector<double> w;
    u64 start_vertex = 0;

    setup(e, w);
    std::cout << e.first.size() << " " << w.size() << std::endl;

    auto G = new cmd::nwgraph(5, 6, std::move(e), std::move(w));

    G->find_max_cliques(start_vertex, true, false);
    auto ans = G->get_max_clique();
    std::cout << "Via Heuristic we get a clique of weight: "
              << G->get_clique_weight(ans) << std::endl;
    for (auto& x : ans) { std::cout << x << " "; }
    std::cout << std::endl;

    start_vertex = 0;

    G->find_max_cliques(start_vertex, false, true);
    auto ans2 = G->get_max_clique();
    std::cout << "Via DFS we get a clique of weight: " << G->get_clique_weight(ans2)
              << std::endl;
    for (auto& x : ans2) { std::cout << x << " "; }
    std::cout << std::endl;

    double clique_weight = G->get_clique_weight(ans2);
    std::cout << "Enumerating all cliques of weight " << clique_weight << std::endl;
    cmd::NWCliqueEnumerator en(clique_weight);
    u64 ct = 0;
    while (true)
    {
        start_vertex = en.process_graph(*G);
        if (start_vertex >= G->n_vert) break;
        ans = G->get_max_clique(start_vertex);
        ct++;
        std::cout << "#" << ct << ": ";
        for (auto x : ans) { std::cout << x << (x == start_vertex ? "!" : " "); }
        std::cout << "\n\n";
    }
    std::cout << ct << " cliques of weight " << clique_weight << std::endl;
    delete G;
}
