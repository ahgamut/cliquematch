#include <core/graph.h>
#include <core/mmio.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "Testing cliquematch core with sample... \n";
    if (argc < 2)
    {
        std::cout << "no file given!!\n";
        return 0;
    }
    std::size_t no_of_vertices, no_of_edges, start_vertex = 0;
    bool heur_done = false, use_heur = false, use_dfs = true;
    double limit = 100;
    std::cout << "Reading from file: " << argv[1] << "\n";
    if (argc >= 3 && std::atoi(argv[2]) == 1)
    {
        std::cout << "Using heuristic\n";
        use_heur = true;
        use_dfs = false;
    }
    auto edges = mmio3_reader(argv[1], no_of_vertices, no_of_edges);
    graph G(no_of_vertices, no_of_edges, edges);
    G.find_max_cliques(start_vertex, heur_done, use_heur, use_dfs, limit);
    auto ans = G.get_max_clique();

    std::cout << G.duration << "s: clique of size: " << ans.size() << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n";
    return 0;
}
