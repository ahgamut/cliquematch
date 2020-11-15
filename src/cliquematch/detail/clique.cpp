/* clique.cpp
 *
 * Contains graph member functions that compute/return cliques.
 * find_max_cliques() is just a wrapper over the preferred order of calling the
 * clique search operations.
 */
#include <detail/graph.h>
#include <detail/dfs.h>
#include <detail/heuristic.h>

namespace cliquematch
{
namespace detail
{
    double graph::find_max_cliques(std::size_t& start_vert, bool use_heur, bool use_dfs,
                                   double time_limit)
    {
        DegreeHeuristic heur;

        this->start_time = std::chrono::steady_clock::now();
        if (use_heur) heur.process_graph((*this));
        if (time_limit <= 0)
        {
            StackDFS dfs;
            if (use_dfs) start_vert = dfs.process_graph((*this));
        }
        else
        {
            RecursionDFS dfs(start_vert, time_limit);
            this->start_time = std::chrono::steady_clock::now();
            if (use_dfs) start_vert = dfs.process_graph((*this));
        }
        return this->elapsed_time();
    }

    std::vector<std::size_t> graph::get_max_clique() const
    {
        return this->get_max_clique(this->CUR_MAX_CLIQUE_LOC);
    }
    std::vector<std::size_t> graph::get_max_clique(std::size_t i) const
    {
        return this->vertices[i].give_clique(this->edge_list.data());
    }
}  // namespace detail
}  // namespace cliquematch
