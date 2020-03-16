#ifndef GRAPH_H
#define GRAPH_H

#include <core/searchstate.h>
#include <core/vertex.h>
#include <set>
#include <vector>

class graph
{
   private:
    std::vector<vertex> vertices;
    void set_vertices();
    // raw list of edges (useless without vertex.elo)
    std::vector<std::size_t> edge_list;
    std::size_t el_size;
    // edges stored as bits for a clique,
    // (padded to ensure 32bit)
    std::vector<u32> edge_bits;
    std::size_t eb_size;
    inline short find_if_neighbors(const vertex& v1, std::size_t v2_id,
                                   std::size_t& v2_position)
    {
        return binary_find(&(this->edge_list[v1.elo]), v1.N, v2_id, v2_position);
    }

   public:
    std::size_t n_vert;
    // runtime heuristics
    double duration;
    std::size_t max_degree;

    std::size_t CUR_MAX_CLIQUE_SIZE;
    std::size_t CUR_MAX_CLIQUE_LOC;
    std::size_t CLIQUE_LIMIT;
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

