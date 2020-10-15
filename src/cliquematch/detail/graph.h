#ifndef GRAPH_H
#define GRAPH_H

#include <detail/vertex.h>
#include <chrono>
#include <functional>
#include <set>
#include <utility>

namespace cliquematch
{
namespace detail
{
    constexpr std::size_t BITS_PER_SIZE_T = 8 * sizeof(std::size_t);
    class graph
    {
       private:
        std::vector<vertex> vertices;
        // raw list of edges (useless without vertex.elo)
        std::vector<std::size_t> edge_list;
        std::size_t el_size;
        // edges stored as bits for a clique,
        // (padded to ensure 32bit)
        std::vector<std::size_t> edge_bits;
        std::size_t eb_size;
        std::size_t search_start, search_cur, search_end;
        std::chrono::time_point<std::chrono::steady_clock> start_time;

        // add stuff for allocating searchstates using existing memory in edge_bits;
        short find_if_neighbors(const std::size_t v1_id, const std::size_t v2_id,
                                std::size_t& v2_position) const
        {
            return binary_find(&(this->edge_list[this->vertices[v1_id].elo]),
                               this->vertices[v1_id].N, v2_id, v2_position);
        }

        void set_vertices();
        void start_clock();
        double elapsed_time() const;
        void clear_memory(const std::size_t N) { search_cur -= N; }
        u32* load_memory(const std::size_t N)
        {
            u32* loc = reinterpret_cast<u32*>(&(edge_bits[search_cur]));
            for (std::size_t i = 0; i < N; i++) edge_bits[search_cur++] = 0;
            return loc;
        }

       public:
        std::size_t n_vert;
        std::size_t max_degree, md_vert;

        std::size_t CUR_MAX_CLIQUE_SIZE;
        std::size_t CUR_MAX_CLIQUE_LOC;
        std::size_t CLIQUE_LIMIT;

        // basic functions
        graph();
        graph(const std::size_t n_vert, const std::size_t n_edges,
              std::pair<std::vector<std::size_t>, std::vector<std::size_t>>&& edges);
        void disp() const;
        void send_data(std::function<void(std::size_t, std::size_t)>) const;

        // giving max_clique to pygraph
        double find_max_cliques(std::size_t& start_vert, bool use_heur = false,
                                bool use_dfs = true, double time_limit = -1);
        std::vector<std::size_t> get_max_clique() const;
        std::vector<std::size_t> get_max_clique(std::size_t i) const;
        std::set<std::size_t> vertex_data(std::size_t i) const;

        friend std::pair<std::vector<std::size_t>, std::vector<std::size_t>> iso_edges(
            std::size_t&, std::size_t&, const graph&, const graph&);
        friend class RecursionDFS;
        friend class StackDFS;
        friend class DegreeHeuristic;
        friend class CliqueEnumerator;
    };
}  // namespace detail
}  // namespace cliquematch

#endif /* GRAPH_H */

