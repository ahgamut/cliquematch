#ifndef PYGRAPH_H
#define PYGRAPH_H

#include <set>
#include <vector>
#include <utility>
#include <cstdint>
#include <cmath>
#include <memory>
#include <pybind11/pybind11.h>
// need to #include<cmath> before pybind11/numpy otherwise issues with ::hypot
#include <pybind11/numpy.h>

#include <detail/graph/graph.h>

#define CM_ERROR(x)                                                                   \
    std::runtime_error((x) + std::string(__FILE__) + " " + std::to_string(__LINE__) + \
                       "\n")


typedef uint64_t u64;
namespace cliquematch
{
namespace core
{
    template <typename dtype>
    using ndarray =
        pybind11::array_t<dtype, pybind11::array::c_style | pybind11::array::forcecast>;

    class CliqueIterator;
    class CorrespondenceIterator;
    class pygraph
    {
       private:
        std::shared_ptr<detail::graph> G;

       public:
        bool finished_all;
        u64 nvert, nedges;
        u64 current_vertex;

        pygraph();
        void load_graph(u64, u64, std::pair<std::vector<u64>, std::vector<u64>>&&);
        void check_loaded() const;
        std::vector<u64> get_max_clique(u64 lower_bound = 1, u64 upper_bound = 0xFFFF,
                                        double time_limit = -1,
                                        bool use_heuristic = true, bool use_dfs = true,
                                        bool continue_search = false);
        void reset_search();
        std::pair<std::vector<u64>, std::vector<u64>> get_correspondence(
            u64 len1, u64 len2, u64 lower_bound = 1, u64 upper_bound = 0xFFFF,
            double time_limit = -1, bool use_heuristic = true, bool use_dfs = true,
            bool continue_search = false);
        std::pair<std::vector<u64>, std::vector<u64>> get_correspondence2(
            u64 len1, u64 len2, std::vector<u64> clique);

        std::string showdata() const;
        ndarray<bool> to_adj_matrix() const;
        ndarray<u64> to_edgelist() const;
        std::vector<std::set<u64>> to_adj_list() const;
        void to_file(std::string filename) const;
        std::set<u64> get_vertex_data(u64 i) const;

        friend std::pair<std::vector<u64>, std::vector<u64>> iso_edges(u64&, u64&,
                                                                       const pygraph&,
                                                                       const pygraph&);
        friend class CliqueIterator;
        friend class CorrespondenceIterator;
    };

    pygraph from_adj_matrix(ndarray<bool> adjmat);
    pygraph from_edgelist(ndarray<u64> edge_list, u64 no_of_vertices);
    pygraph from_file(std::string filename);
    pygraph from_adj_list(u64 n_vertices, u64 n_edges,
                          std::vector<std::set<u64>> edges);
}  // namespace core
}  // namespace cliquematch
#endif /* PYGRAPH_H */
