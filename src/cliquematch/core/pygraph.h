#ifndef PYGRAPH_H
#define PYGRAPH_H

#define CM_ERROR(x)                                                                   \
    std::runtime_error((x) + std::string(__FILE__) + " " + std::to_string(__LINE__) + \
                       "\n")

#include <core/graph.h>
#include <cmath>
// need to #include<cmath> before pybind11/numpy otherwise issues with ::hypot
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
template <typename dtype>
using ndarray =
    pybind11::array_t<dtype, pybind11::array::c_style | pybind11::array::forcecast>;

class pygraph
{
   private:
    graph G;
    bool ans_found;
    void find_max_clique();
    std::vector<std::size_t> ans_clique;

   protected:
    void load_graph(std::size_t, std::size_t, std::vector<std::set<std::size_t>>);
    bool finished_heur;

   public:
    bool use_heur;
    bool use_dfs;
    bool finished_all;
    double time_lim;
    std::size_t nvert, nedges;
    std::size_t lower_bound, upper_bound;
    std::size_t current_vertex;

    pygraph();
    pygraph(std::size_t, std::size_t, std::vector<std::set<std::size_t>>);
    void continue_search();
    void reset_search();
    std::vector<std::size_t> get_max_clique();
    std::string showdata();

    ndarray<bool> to_adj_matrix();
    ndarray<std::size_t> to_edgelist();
    std::vector<std::set<std::size_t>> to_adj_list();
    void to_file(std::string filename);

    friend std::vector<std::set<std::size_t>> iso_edges(std::size_t&, std::size_t&,
                                                        const pygraph&, const pygraph&);
};

pygraph from_adj_matrix(ndarray<bool> adjmat);
pygraph from_edgelist(ndarray<std::size_t> edge_list, std::size_t no_of_vertices);
pygraph from_file(std::string filename);
pygraph from_adj_list(std::size_t n_vertices, std::size_t n_edges,
                      std::vector<std::set<std::size_t>> edges);
#endif /* PYGRAPH_H */

