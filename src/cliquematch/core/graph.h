#ifndef GRAPH_H
#define GRAPH_H

#include <core/vertex.h>
#include <chrono>
#include <set>

class graph
{
   private:
    std::vector<vertex> vertices;
    std::vector<std::size_t> indices;  // for sorting
    // raw list of edges (useless without vertex.elo)
    std::vector<std::size_t> edge_list;
    std::size_t el_size;
    // edges stored as bits for a clique,
    // (padded to ensure 32bit)
    std::vector<u32> edge_bits;
    std::size_t eb_size;
    std::chrono::time_point<std::chrono::steady_clock> start_time;

    short find_if_neighbors(const vertex&, std::size_t, std::size_t&);
    double elapsed_time();
    void set_vertices();

   public:
    std::size_t n_vert;

    // runtime heuristics
    std::size_t max_degree;
    std::size_t CUR_MAX_CLIQUE_SIZE;
    std::size_t CUR_MAX_CLIQUE_LOC;
    std::size_t CLIQUE_LIMIT;
    double duration;
    double TIME_LIMIT;

    // basic functions
    graph();
    graph(std::size_t n_vert, std::size_t n_edges,
          std::vector<std::set<std::size_t>>& edges, std::size_t clique_lim = 1000);
    void disp();

    // giving max_clique to pygraph
    void find_max_cliques(std::size_t& start_vert, bool& heur_done,
                          bool use_heur = false, bool use_dfs = true,
                          double time_limit = 10000);
    std::vector<std::size_t> get_max_clique(std::size_t i);
    std::vector<std::size_t> get_max_clique();

    // finding ONE max clique
    void dfs_one_search(std::size_t cur, const graphBits&, const graphBits&);
    void dfs_one_clique(std::size_t cur);
    std::size_t dfs_all_cliques(std::size_t start_vertex = 0, double time_limit = 1000);

    // using the heuristic for finding ONE clique
    void heur_one_clique(std::size_t cur);
    std::size_t heur_all_cliques(std::size_t start_vertex = 0, double time_limit = 100);

    // finding OTHER max cliques
    void dfs_other_search(std::size_t cur, const graphBits&, const graphBits&);
    void dfs_other_clique(std::size_t cur);
    void dfs_other_clique(std::size_t cur, std::size_t limit);
    std::vector<std::size_t> possible_others();
};

#endif /* GRAPH_H */

