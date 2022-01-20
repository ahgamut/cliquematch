#include <detail/nwgraph/nwgraph.h>
#include <detail/nwgraph/nwclique.h>
#include <detail/mmio.h>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

namespace cmd = cliquematch::detail;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "cm_nw <mtx file>" << std::endl;
        return 1;
    }
    std::vector<double> weights;
    u64 start_vertex = 0, no_of_vertices = 0, no_of_edges = 0;

    auto start = std::chrono::steady_clock::now();
    auto edges = cmd::mmio4_reader(argv[1], no_of_vertices, no_of_edges);
    weights.reserve(no_of_vertices + 1);
    weights.push_back(0);
    for (u64 i = 1; i <= no_of_vertices; i++)
    {
        weights.push_back(1);
        edges.first.push_back(i);
        edges.second.push_back(i);
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    auto reading_time = static_cast<double>(elapsed.count()) / 1e6;

    start = std::chrono::steady_clock::now();
    auto G = new cmd::nwgraph(no_of_vertices, no_of_edges, std::move(edges),
                              std::move(weights));
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    auto loading_time = static_cast<double>(elapsed.count()) / 1e6;
    std::cout << "File reading took " << reading_time << "s" << std::endl;
    std::cout << "Graph loading took " << loading_time << "s" << std::endl;

    start = std::chrono::steady_clock::now();
    G->find_max_cliques(start_vertex, true, false);
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    auto heuristic_time = static_cast<double>(elapsed.count()) / 1e6;
    auto ans = G->get_max_clique();
    std::cout << heuristic_time << "s: Via Heuristic we get a clique of weight: "
              << G->get_clique_weight(ans) << std::endl;
    for (auto& x : ans) { std::cout << x << " "; }
    std::cout << std::endl;

    start_vertex = 0;

    start = std::chrono::steady_clock::now();
    G->find_max_cliques(start_vertex, false, true);
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    auto dfs_time = static_cast<double>(elapsed.count()) / 1e6;
    auto ans2 = G->get_max_clique();
    std::cout << dfs_time
              << "s: Via DFS we get a clique of weight: " << G->get_clique_weight(ans2)
              << std::endl;
    for (auto& x : ans2) { std::cout << x << " "; }
    std::cout << std::endl;

    double clique_weight = G->get_clique_weight(ans2) - 1;
    std::cout << "Enumerating all cliques of weight " << clique_weight << std::endl;
    cmd::NWCliqueEnumerator en(clique_weight);
    u64 ct = 0;
    while (true)
    {
        start_vertex = en.process_graph(*G);
        if (start_vertex >= G->n_vert) break;
        ans = G->get_max_clique(start_vertex);
        if (G->get_clique_weight(ans) >= clique_weight)
        {
            ct++;
            /*
            std::cout << "#" << ct << ": " << ans.size() << "/"
                      << G->get_clique_weight(ans) << " : ";
            for (auto x : ans) { std::cout << x << (x == start_vertex ? "!" : " "); }
            std::cout << "\n\n";
            */
        }
    }
    std::cout << ct << " cliques of weight " << clique_weight << std::endl;
    delete G;
}
