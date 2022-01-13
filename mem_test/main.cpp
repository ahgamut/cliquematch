#include <detail/graph/graph.h>
#include <detail/graph/mmio.h>
#include <detail/graph/dfs.h>
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
    cmd::u64 no_of_vertices, no_of_edges, start_vertex = 0,
                                                          clique_size = 0;
    double reading_time, loading_time, clique_time;
    std::cout << "Reading from file: " << argv[1] << "\n";

    auto start = std::chrono::steady_clock::now();
    auto edges = cmd::mmio4_reader(argv[1], no_of_vertices, no_of_edges);
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    reading_time = static_cast<double>(elapsed.count()) / 1e6;

    start = std::chrono::steady_clock::now();
    cmd::graph* G = new cmd::graph(no_of_vertices, no_of_edges, std::move(edges));
    // cmd::graph* G = new cmd::graph(no_of_vertices, no_of_edges, edges);
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now() - start);
    loading_time = static_cast<double>(elapsed.count()) / 1e6;
    std::cout << "File reading took " << reading_time << "s" << std::endl;
    std::cout << "Graph loading took " << loading_time << "s" << std::endl;

    clique_time = G->find_max_cliques(start_vertex, true, false, -1);
    auto ans = G->get_max_clique();
    std::cout << "Clique finding (via heuristic) took " << clique_time
              << "s\nclique of size: " << ans.size() << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n\n";

    G->CUR_MAX_CLIQUE_SIZE = 1;
    G->CUR_MAX_CLIQUE_LOCATION = start_vertex = 0;
    clique_time = G->find_max_cliques(start_vertex, false, true, 10);
    ans = G->get_max_clique();
    std::cout << "Clique finding (via recursion) took " << clique_time
              << "s\nclique of size: " << ans.size() << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n\n";

    G->CUR_MAX_CLIQUE_SIZE = 1;
    G->CUR_MAX_CLIQUE_LOCATION = start_vertex = 0;
    clique_time = G->find_max_cliques(start_vertex, false, true, -1);
    ans = G->get_max_clique();
    std::cout << "Clique finding (via stack) took " << clique_time
              << "s\nclique of size: " << ans.size() << "\n";
    for (auto x : ans) { std::cout << x << " "; }
    std::cout << "\n\n";

    clique_size = G->CUR_MAX_CLIQUE_SIZE - 1;
    std::cout << "Enumerating all cliques of size " << clique_size << std::endl;
    cmd::CliqueEnumerator en(clique_size);
    cmd::u64 ct = 0;
    while (true)
    {
        start_vertex = en.process_graph(*G);
        if (start_vertex >= G->n_vert) break;
        ans = G->get_max_clique(start_vertex);
        if (ans.size() == clique_size) ct++;
        /*  std::cout << "#" << ct << ": ";
          for (auto x : ans) { std::cout << x << (x == start_vertex ? "!" : " "); }
          std::cout << "\n\n";*/
    }
    std::cout << ct << " cliques of size " << clique_size << std::endl;
    delete G;
    return 0;
}

