#ifndef PYNWGRAPH_H
#define PYNWGRAPH_H

#include <detail/nwgraph/nwgraph.h>
#include <core/core.h>

namespace cliquematch
{
namespace core
{
    class NWCliqueIterator;
    class NWCorrespondenceIterator;
    class pynwgraph
    {
       private:
        std::shared_ptr<detail::nwgraph> G;

       public:
        u64 nvert, nedges;
        u64 current_vertex;
        bool finished_all;

        pynwgraph();
        void load_graph(u64, u64, std::pair<std::vector<u64>, std::vector<u64>>&&,
                        std::vector<double>&&);
        void check_loaded() const;

        void reset_search();
        std::vector<u64> get_max_clique(double lower_bound = 1,
                                        double upper_bound = 0xFFFF,
                                        bool use_heuristic = true, bool use_dfs = true);

        std::pair<std::vector<u64>, std::vector<u64>> get_correspondence(
            u64 len1, u64 len2, double lower_bound = 1, double upper_bound = 0xFFFF,
            bool use_heuristic = true, bool use_dfs = true);
        std::pair<std::vector<u64>, std::vector<u64>> get_correspondence2(
            u64 len1, u64 len2, std::vector<u64> clique);

        double get_clique_weight(std::vector<u64>& clique) const;
        std::vector<double> get_all_weights() const;

        ndarray<bool> to_adj_matrix() const;
        ndarray<u64> to_edgelist() const;
        std::vector<std::set<u64>> to_adj_list() const;
        std::pair<double, std::set<u64>> get_vertex_data(u64 i) const;

        std::string showdata() const;
        friend class NWCliqueIterator;
        friend class NWCorrespondenceIterator;
    };

    pynwgraph nw_from_adj_matrix(ndarray<bool> adjmat, ndarray<double> weights);
    pynwgraph nw_from_edgelist(ndarray<u64> edge_list, u64 no_of_vertices,
                               ndarray<double> weights);
    pynwgraph nw_from_adj_list(u64 n_vertices, u64 n_edges,
                               std::vector<std::set<u64>> edges,
                               std::vector<double> weights);

}  // namespace core
}  // namespace cliquematch

#endif /* PYNWGRAPH_H */
