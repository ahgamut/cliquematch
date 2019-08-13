#ifndef PYGRAPH_H
#define PYGRAPH_H

#include <core/graph.h>
#include <core/mmio.h>
#include <cmath>
// need to #include<cmath> before pybind11/numpy otherwise issues with ::hypot
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
template <typename dtype>
using ndarray = pybind11::array_t<dtype, pybind11::array::c_style |
                                             pybind11::array::forcecast>;

//' The basic data structure used to store a sparse graph
//' and share info with python
struct pygraph {
    graph G;
    std::vector<std::set<u32>> EDGES;
    u32 nvert, nedges;

    std::vector<u32> ans_clique;

    unsigned int lower_bound, upper_bound;
    unsigned int current_vertex;

    bool use_heur;
    bool use_dfs;
    bool finished_heur;
    bool finished_all;
    bool ans_found;
    double time_lim;

    pygraph();
    pygraph(ndarray<bool> adjmat);
    pygraph(ndarray<unsigned int> edge_list, unsigned int no_of_vertices);
    pygraph(std::string filename, unsigned int reader_choice);
    void load_graph();
    void find_max_clique();
    void continue_search();
    void reset_search();
    std::vector<u32> get_max_clique();
    std::string showdata();
};

#endif /* PYGRAPH_H */

