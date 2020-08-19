#include <core/graph.h>
#include <core/mmio.h>
#include <cstdlib>
#include <chrono>
#include <iostream>

namespace cmd = cliquematch::detail;

int main(int argc, char* argv[])
{
    std::cout << "Testing cliquematch core with sample... \n";
    if (argc < 2)
    {
        std::cout << "no file given!!\n";
        return 0;
    }
    std::size_t no_of_vertices, no_of_edges, start_vertex = 0;
    bool heur_done = false, use_heur = false, use_dfs = true;
    double limit = 10000, reading_time, loading_time, clique_time;
    std::cout << "Reading from file: " << argv[1] << "\n";

    auto start = std::chrono::steady_clock::now();
    auto edges = cmd::mmio3_reader(argv[1], no_of_vertices, no_of_edges);
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    reading_time = static_cast<double>(elapsed.count()) / 1e6;

    start = std::chrono::steady_clock::now();
    cmd::graph* G = new cmd::graph(no_of_vertices, no_of_edges, edges);
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    loading_time = static_cast<double>(elapsed.count()) / 1e6;

    if (argc >= 3 && std::atoi(argv[2]) == 1)
    {
        std::cout << "Using heuristic\n";
        use_heur = true;
        use_dfs = false;
    }
    clique_time =
        G->find_max_cliques(start_vertex, heur_done, use_heur, use_dfs, limit);
    auto ans = G->get_max_clique();

    std::cout << "File reading took " << reading_time << "s" << std::endl;
    std::cout << "Graph loading took " << loading_time << "s" << std::endl;
    std::cout << "Clique finding took " << clique_time
              << "s\nclique of size: " << G->CUR_MAX_CLIQUE_SIZE << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n\n";
    delete G;
    return 0;
}

