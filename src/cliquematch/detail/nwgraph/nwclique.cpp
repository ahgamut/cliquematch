#include <detail/nwgraph/nwgraph.h>
#include <detail/nwgraph/nwclique.h>

namespace cliquematch
{
namespace detail
{
    void nwgraph::find_max_cliques(u64& start_vert, bool use_heur, bool use_dfs)
    {
        NWHeuristic heur;
        NWStackDFS dfs;

        if (use_heur) heur.process_graph((*this));
        if (use_dfs) start_vert = dfs.process_graph((*this));
    }

    std::vector<u64> nwgraph::get_max_clique() const
    {
        return this->get_max_clique(this->CUR_MAX_CLIQUE_LOCATION);
    }
    std::vector<u64> nwgraph::get_max_clique(u64 i) const
    {
        return this->vertices[i].give_clique(this->edge_list.data());
    }
    double nwgraph::get_clique_weight(const std::vector<u64>& clique) const
    {
        double wt = 0;
        for (auto v : clique)
        {
            if (v < this->n_vert) wt += vertices[v].weight;
        }
        return wt;
    }
}  // namespace detail
}  // namespace cliquematch
