// this hpp contains templates included in multiple files
// keep a ifndef guard to prevent excesses

#ifndef EXT_TEMPLATE_HPP
#define EXT_TEMPLATE_HPP

#include <templates/ext_template.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;
// template syntax brain hurty

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::GraphTemplate() {
    this->nvert = 0;
    this->nedges = 0;
    this->use_heur = false;
    this->use_dfs = false;
    this->lower_bound = 0;
    this->upper_bound = 32;
    this->time_lim = 100;
    this->finished_heur = false;
    this->finished_all = false;
    this->current_vertex = 0;
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::GraphTemplate(
    List1& pts1, u32 pts1_len, List2& pts2, u32 pts2_len,
    std::function<Delta1(List1&, u32, u32)> d1, bool is_d1_symmetric,
    std::function<Delta2(List2&, u32, u32)> d2, bool is_d2_symmetric)
    : GraphTemplate<List1, Delta1, List2, Delta2, EpsType>() {
    this->ps1 = relset<List1, Delta1>(pts1_len, d1, is_d1_symmetric);
    this->ps2 = relset<List2, Delta2>(pts2_len, d2, is_d2_symmetric);
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
bool GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::build_edges(
    List1& pts1, List2& pts2) {
    this->ps1.fill_dists(pts1);
    this->ps2.fill_dists(pts2);
    unsigned int no_of_vertices, no_of_edges;
    auto EDGES = edges_from_relsets(no_of_vertices, no_of_edges, this->ps1,
				    this->ps2, this->pts_epsilon);
    if (EDGES.data() == nullptr || EDGES.size() == 0) {
	throw std::runtime_error("Could not extract edges" +
				 std::string(__FILE__) + "  " +
				 std::to_string(__LINE__) + "\n");
    }

    this->load_graph(no_of_vertices, no_of_edges, EDGES);
    return true;
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
bool GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::
    build_edges_with_condition(
	List1& pts1, List2& pts2,
	std::function<bool(List1&, u32, u32, List2&, u32, u32)> cfunc,
	bool use_cfunc_only) {
    unsigned int no_of_vertices, no_of_edges;
    auto cfwrap = [&pts1, &pts2, &cfunc](u32 i, u32 j, u32 i2, u32 j2) -> bool {
	return cfunc(pts1, i, j, pts2, i2, j2);
    };

    if (!use_cfunc_only) {
	this->ps1.fill_dists(pts1);
	this->ps2.fill_dists(pts2);
    }
    auto edges =
	efr_condition(no_of_vertices, no_of_edges, this->ps1, this->ps2,
		      this->pts_epsilon, cfwrap, use_cfunc_only);
    if (edges.data() == nullptr || edges.size() == 0) {
	throw std::runtime_error("Could not extract edges" +
				 std::string(__FILE__) + "  " +
				 std::to_string(__LINE__) + "\n");
    }

    this->load_graph(no_of_vertices, no_of_edges, edges);
    return true;
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
py::list GraphTemplate<List1, Delta1, List2, Delta2,
		       EpsType>::get_correspondence2(std::vector<u32> clique) {
    py::list a1;
    py::list a2;

    u32 i, t1, t2;
    for (i = 0; i < clique.size(); i++) {
	if (clique[i] == 0) {
	    throw std::runtime_error(
		"Sentinel Value (0) appeared in clique. Increase the time "
		"limit; if repeated, reset graph\n" +
		std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
	}
	t1 = (clique[i] - 1) / ps2.N;
	t2 = (clique[i] - 1) % ps2.N;
	a1.append(t1);
	a2.append(t2);
    }
    py::list ans;

    ans.append(a1);
    ans.append(a2);

    return ans;
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
py::list
GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::get_correspondence() {
    return this->get_correspondence2(this->get_max_clique());
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
std::string GraphTemplate<List1, Delta1, List2, Delta2, EpsType>::showdata() {
    std::stringstream ss;
    ss << "cliquematch.cm_base.<templated class object> at " << this << "\n";
    ss << "epsilon = " << this->pts_epsilon << "\n";

    if (this->nvert != 0) {
	ss << "Correspondence graph has " << this->nvert << " vertices, "
	   << this->nedges << " edges\n";
	ss << "Search Parameters : \n";
	ss << "Bounds: [" << this->lower_bound << "," << this->upper_bound
	   << "]"
	   << "\nTime limit: " << this->time_lim << "s\n";
	ss << "Using Heuristic : " << (this->use_heur ? "True" : "False");
	ss << " Using DFS: " << (this->use_dfs ? "True" : "False");
	ss << "\n";
	ss << "Search Completed? : " << (this->finished_all ? "True" : "False")
	   << "\n";
    }
    return ss.str();
}

#endif /* EXT_TEMPLATE_HPP */

