/* clique.cpp
 *
 * Contains graph member functions that compute/return cliques.
 * find_max_cliques() is just a wrapper over the preferred order of calling the
 * clique search operations.
 */
#include <detail/graph/graph.h>
#include <detail/graph/dfs.h>
#include <detail/graph/heuristic.h>

namespace cliquematch
{
namespace detail
{
    double graph::find_max_cliques(u64& start_vert, bool use_heur, bool use_dfs,
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

    std::vector<u64> graph::get_max_clique() const
    {
        return this->get_max_clique(this->CUR_MAX_CLIQUE_LOCATION);
    }
    std::vector<u64> graph::get_max_clique(u64 i) const
    {
        return this->vertices[i].give_clique(this->edge_list.data());
    }
}  // namespace detail
}  // namespace cliquematch
