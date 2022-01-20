#ifndef NWGRAPH_H
#define NWGRAPH_H

#include <detail/nwgraph/wvertex.h>
#include <functional>
#include <set>
#include <utility>

namespace cliquematch
{
namespace detail
{
    class nwgraph
    {
       private:
        std::vector<u64> edge_list;     // store raw list of edges
        std::vector<u64> edge_bits;     // store all clique bitsets (padded)
        std::vector<wvertex> vertices;  // store all contextual vertex info
        u64 el_size;                    // edge_list.size(), computed separately
        u64 eb_size;                    // amount of memory used for bitsets

        // to handle the extra memory available in edge_bits
        u64 search_start, search_cur, search_end;
        u64* load_memory(const u64 N)
        {
            /* Take available memory owned by edge_bits and return it
             * to a graphBits object to use during the clique search. It's not
             * nice, but it allows avoiding repeated small allocations on
             * the heap. Clear the memory manually and reuse.
             *
             * This can be done in a smarter way (a custom memory allocator
             * either globally or within the graph), but this is fast enough
             * for now. Check graph.cpp and dfs_stack.cpp for more context.
             */
            u64* loc = &(edge_bits[search_cur]);
            search_cur += N;
            return loc;
        }
        void clear_memory(const u64 N)
        {
            search_cur -= N;
            for (u64 i = 0; i < N; i++) edge_bits[search_cur + i] = 0;
        }

        // find if two vertices are neighbors, if yes, return valid position
        BFResult find_if_neighbors(const u64 v1_id, const u64 v2_id,
                                   u64& v2_position) const
        {
            return binary_find(&(this->edge_list[this->vertices[v1_id].elo]),
                               this->vertices[v1_id].N, v2_id, v2_position);
        }

        void set_bounds();  // used by constructor to set bounds for search

       public:
        u64 n_vert;     // number of vertices in the graph
        u64 max_depth;  // the maximum depth of the search stack
        u64 max_neighbors;
        double max_degree;
        double CLIQUE_LIMIT;          // upper bound on the clique size for this graph
        double CUR_MAX_CLIQUE_SIZE;   // size of the CUR_MAX_CLIQUE
        u64 CUR_MAX_CLIQUE_LOCATION;  // vertex ID containing the CUR_MAX_CLIQUE

        // basic functions
        nwgraph();
        // notice that edges are consumed (CALLER needs to std::move)
        nwgraph(const u64 n_vert, const u64 n_edges,
                std::pair<std::vector<u64>, std::vector<u64>>&& edges,
                std::vector<double>&& weights);

        // call with starting vertex to compute cliques
        void find_max_cliques(u64& start_vert, bool use_heur = false,
                              bool use_dfs = true);
        // return the overall max clique
        std::vector<u64> get_max_clique() const;
        // return the max clique stored for vertex i
        std::vector<u64> get_max_clique(u64) const;
        // compute the weight of a given clique
        double get_clique_weight(const std::vector<u64>&) const;
        // return all neighbors of a vertex
        std::set<u64> vertex_data(u64) const;
        // pass edges one by one to external function
        void send_data(std::function<void(u64, u64)>) const;

        // display data for debugging
        void disp() const;

        // clique search objects need access to internal data
        friend class NWStackDFS;
        friend class NWHeuristic;
        friend class NWCliqueEnumerator;
    };
}  // namespace detail
}  // namespace cliquematch

#endif /* NWGRAPH_H */
