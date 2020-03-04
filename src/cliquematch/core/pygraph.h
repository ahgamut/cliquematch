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

class pygraph {
   private:
    graph G;
    bool ans_found;
    void find_max_clique();
    std::vector<unsigned int> ans_clique;

   protected:
    void load_graph(unsigned int, unsigned int,
		    std::vector<std::set<unsigned int>>);
    bool finished_heur;

   public:
    std::vector<std::set<u32>> EDGES;
    bool use_heur;
    bool use_dfs;
    bool finished_all;
    double time_lim;
    unsigned int nvert, nedges;
    unsigned int lower_bound, upper_bound;
    unsigned int current_vertex;

    pygraph();
    pygraph(unsigned int, unsigned int, std::vector<std::set<unsigned int>>);
    void continue_search();
    void reset_search();
    std::vector<u32> get_max_clique();
    std::string showdata();
};

pygraph from_adj_matrix(ndarray<bool> adjmat);
pygraph from_edgelist(ndarray<unsigned int> edge_list,
		      unsigned int no_of_vertices);
pygraph from_file(std::string filename, unsigned int reader_choice);
#endif /* PYGRAPH_H */

