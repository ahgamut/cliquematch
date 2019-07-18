#ifndef GRAPH2_H
#define GRAPH2_H

#include <cmath>
// need to #include<cmath> before pybind11/numpy otherwise issues with ::hypot
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
template <typename dtype>
using ndarray = pybind11::array_t<dtype, pybind11::array::c_style |
                                             pybind11::array::forcecast>;

#include <cm_base/core/graph.h>
#include <cm_base/core/mmio.h>

//' The basic data structure used to store a sparse graph
//' and share info with python
struct pygraph {
    graph G;
    std::vector<std::set<u32>> EDGES;
    u32 nvert, nedges;

    std::vector<u32> ans_clique;
    bool ans_found = false;

    bool use_heur;
    bool use_dfs;
    unsigned int clique_lim;
    double time_lim;

    unsigned int current_vertex;
    bool finished_heur;
    bool finished_all;

    pygraph();
    pygraph(ndarray<bool> adjmat);
    // pygraph(std::vector<std::set<u32> > adjlist, unsigned int no_of_vertices);
    pygraph(ndarray<unsigned int> edge_list, unsigned int no_of_vertices);
    pygraph(std::string filename, unsigned int reader_choice);
    void find_max_clique();
    void continue_search();
    std::vector<u32> get_max_clique();
    std::string showdata();
};

template <class T = double>
pybind11::list return_doubleton(T a, T b) {
    pybind11::list z;
    z.append(a);
    z.append(b);
    return z;
}

#endif /* GRAPH2_H */

