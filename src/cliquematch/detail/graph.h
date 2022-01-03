#ifndef GRAPH_H
#define GRAPH_H

/* graph.h
 *
 * The graph class encapsulates all the data required for finding cliques in a
 * graph. It is a struct-of-4-arrays:
 *
 * 1) the raw edge data, 2) the contextual vertex data, 3) the bitset data for
 * cliques at each vertex, and 4) bitset space for the clique search
 *
 * There are additional members for helping the clique search and checking
 * time. The actual clique search methods have been moved into separate friend
 * classes. This class only provides access to the clique data and returns
 * cliques.
 *
 * Usage: default initialize if needed, initialize using constructor and edges,
 * call find_max_cliques or pass to clique search operators, send_data to print
 * or get vertex_data if needed, cleanup handled by std::vector.
 *
 * Design quirks:
 *
 * The constructor accepts a pair<vector>&& instead of vector<pair>& so that
 * the memory used for reading the edges can be swapped into the graph. This
 * saves a LOT of memory, but the tradeoff is the use of raw pointers, and
 * custom memory management.

 * Earlier (4) was allocated on the heap, but now has been combined with (3).
 * There is some internal bookkeeping done for the clique search, but overall
 * memory is managed by std::vector<T> and member functions DO NOT perform any
 * new/deletes.
 */

#include <detail/vertex.h>
#include <chrono>
#include <functional>
#include <set>
#include <utility>

namespace cliquematch
{
namespace detail
{
    // to help CALLERs of load_memory/clear_memory
    constexpr u64 BITS_PER_U64 = 8 * sizeof(u64);

    class graph
    {
       private:
        std::vector<u64> edge_list;    // store raw list of edges
        std::vector<u64> edge_bits;    // store all clique bitsets (padded)
        std::vector<vertex> vertices;  // store all contextual vertex info
        u64 el_size;                   // edge_list.size(), computed separately
        u64 eb_size;                   // amount of memory used for bitsets

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

        // to handle checking the time during the search
        std::chrono::time_point<std::chrono::steady_clock> start_time;
        void start_clock() { this->start_time = std::chrono::steady_clock::now(); }
        double elapsed_time() const
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - this->start_time);
            return static_cast<double>(elapsed.count()) / 1e6;
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
        u64 n_vert;               // number of vertices in the graph
        u64 max_degree, md_vert;  // md_vert = vertex that has max_degree

        // in early implementations these were global variables within
        // graph.cpp, i.e. it is a big deal when these change during the search

        u64 CLIQUE_LIMIT;         // upper bound on the clique size for this graph
        u64 CUR_MAX_CLIQUE_SIZE;  // size of the CUR_MAX_CLIQUE
        u64 CUR_MAX_CLIQUE_LOC;   // vertex ID containing the CUR_MAX_CLIQUE

        // basic functions
        graph();
        // notice that edges are consumed (CALLER needs to std::move)
        graph(const u64 n_vert, const u64 n_edges,
              std::pair<std::vector<u64>, std::vector<u64>>&& edges);

        // call with starting vertex to compute cliques
        double find_max_cliques(u64& start_vert, bool use_heur = false,
                                bool use_dfs = true, double time_limit = -1);
        // return the overall max clique
        std::vector<u64> get_max_clique() const;
        // return the max clique stored for vertex i
        std::vector<u64> get_max_clique(u64 i) const;
        // return all neighbors of a vertex
        std::set<u64> vertex_data(u64 i) const;
        // pass edges one by one to external function
        void send_data(std::function<void(u64, u64)>) const;

        // display data for debugging
        void disp() const;

        // construct a correspondence graph for subgraph isomorphisms
        friend std::pair<std::vector<u64>, std::vector<u64>> iso_edges(u64&, u64&,
                                                                       const graph&,
                                                                       const graph&);

        // clique search objects need access to internal data
        friend class RecursionDFS;
        friend class StackDFS;
        friend class DegreeHeuristic;
        friend class CliqueEnumerator;
    };
}  // namespace detail
}  // namespace cliquematch

#endif /* GRAPH_H */
