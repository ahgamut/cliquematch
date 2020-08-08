#include <core/graph.h>
#include <core/dfs.h>
#include <core/heuristic.h>

namespace cliquematch
{
namespace detail
{
    double graph::find_max_cliques(std::size_t& start_vert, bool& heur_done,
                                   bool use_heur, bool use_dfs, double time_limit)
    {
#if STACK_DFS == 1
        StackDFS dfs;
#else
        RecursionDFS dfs;
#endif
        DegreeHeuristic heur;
        this->start_time = std::chrono::steady_clock::now();
        if (!heur_done && use_heur)
            start_vert = heur.process_graph((*this), start_vert, time_limit);

        if (this->elapsed_time() < time_limit)
        {
            if (!heur_done) start_vert = 0;
            heur_done = true;
            if (use_dfs)
                start_vert = dfs.process_graph((*this), start_vert, time_limit);
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
