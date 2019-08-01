#ifndef GRAPH_H
#define GRAPH_H

#include <core/searchstate.h>
#include <core/vertex.h>
#include <set>
#include <vector>

struct graph {
    std::vector<vertex> vertices;  // list of of vertices
    vertex* V;                     // usually vertices.data()
    u32 n_vert;  // number of vertices (expect an off-by-one glitch?)

    std::vector<u32>
        edge_list;  // raw list of edges (useless without vertex.elo)
    u32* el_base;   // usually edge_list.data()
    u32 el_size;    // number of edges (expect an off-by-k glitch? (K = no. of
                    // isolated vertices +1)

    std::vector<u32> edge_bits;  // edges stored as bits for a clique, (padded
                                 // to ensure 32bit)
    u32* eb_base;                // edge_bits.data()
    u32 eb_size;  // number of bits required for edge-list (+ padding per
                  // vertex)

    // runtime heuristics
    u32 max_degree;
    double duration;

    u32 CUR_MAX_CLIQUE_SIZE;
    u32 CUR_MAX_CLIQUE_LOC;
    u32 CLIQUE_LIMIT;
    double TIME_LIMIT;

    inline short find_if_neighbors(const vertex& v1, u32 v2_id,
                                   u32& v2_position) {
        return binary_find(this->el_base + v1.elo, v1.N, v2_id, v2_position);
    }

    // basic functions
    graph();
    graph(u32 n_vert, u32 n_edges, std::vector<std::set<u32>>& edges,
          u32 clique_lim = 1000);
    void set_vertices();
    void disp();
    // giving max_clique to pygraph
    void find_max_cliques(u32& start_vert, bool& heur_done,
                          bool use_heur = false, bool use_dfs = true,
                          double time_limit = 10000);
    std::vector<u32> get_max_clique(u32 i);
    std::vector<u32> get_max_clique();

    // finding ONE max clique
    void dfs_one_search(u32 cur, const graphBits&, const graphBits&);
    void dfs_one_clique(u32 cur);
    u32 dfs_all_cliques(u32 start_vertex = 0, double time_limit = 1000);

    // using the heuristic for finding ONE clique
    void heur_one_clique(u32 cur);
    u32 heur_all_cliques(u32 start_vertex = 0, double time_limit = 100);

    // finding OTHER max cliques
    void dfs_other_search(u32 cur, const graphBits&, const graphBits&);
    void dfs_other_clique(u32 cur);
    void dfs_other_clique(u32 cur, u32 limit);
    std::vector<u32> possible_others();
};

#endif /* GRAPH_H */

