#include <core/graph.h>
#include <core/mmio.h>
#include <cstdlib>
#include <chrono>
#include <iostream>

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
    double limit = 10000;
    std::cout << "Reading from file: " << argv[1] << "\n";
    if (argc >= 3 && std::atoi(argv[2]) == 1)
    {
        std::cout << "Using heuristic\n";
        use_heur = true;
        use_dfs = false;
    }

    auto start = std::chrono::steady_clock::now();
    auto edges = mmio3_reader(argv[1], no_of_vertices, no_of_edges);
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    double diff = static_cast<double>(elapsed.count()) / 1e6;
    std::cout << "File reading took " << diff << "s" << std::endl;

    start = std::chrono::steady_clock::now();
    graph* G = new graph(no_of_vertices, no_of_edges, edges);
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    diff = static_cast<double>(elapsed.count()) / 1e6;
    std::cout << "Graph loading took " << diff << "s" << std::endl;

    double time_taken =
        G->find_max_cliques(start_vertex, heur_done, use_heur, use_dfs, limit);
    auto ans = G->get_max_clique();

    std::cout << time_taken << "s: clique of size: " << G->CUR_MAX_CLIQUE_SIZE << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n";
    delete G;
    return 0;
}

