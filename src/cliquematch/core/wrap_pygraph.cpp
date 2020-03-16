#include <core/pygraph.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

namespace p = pybind11;
using namespace pybind11;

void init_pygraph(p::module& m)
{
    m.def("graph_from_file", &from_file, arg("filename"), arg("type"));
    m.def("graph_from_adjmat", &from_adj_matrix, arg("adjmat"));
    m.def("graph_from_edgelist", &from_edgelist, arg("edgelist"), arg("num_vertices"));
    class_<pygraph>(m, "Graph")
        .def(init<>())
        .def(init<std::size_t, std::size_t, std::vector<std::set<std::size_t>>>(),
             arg("num_vertices"), arg("num_edges"), arg("edges"))
        .def_readwrite("use_heuristic", &pygraph::use_heur,
                       "Search using the heuristic if true")
        .def_readwrite("use_dfs", &pygraph::use_dfs,
                       "Perform the depth-first search if true")
        .def_readwrite("lower_bound", &pygraph::lower_bound,
                       "Set the lower bound on the size of clique to find")
        .def_readwrite("upper_bound", &pygraph::upper_bound,
                       "Set the upper bound on the size of clique to find")
        .def_readwrite("time_limit", &pygraph::time_lim,
                       "Set the time limit on the search")
        .def_readonly("current_vertex", &pygraph::current_vertex,
                      "The vertex which is about to be searched (Readonly)")
        .def_readonly("search_done", &pygraph::finished_all,
                      "Whether the search has been completed (Readonly)")
        .def_readonly("n_vertices", &pygraph::nvert,
                      "Number of vertices in the graph (Readonly)")
        .def_readonly("n_edges", &pygraph::nedges,
                      "Number of edges in the graph (Readonly)")
        .def_property_readonly("adjacency_list", [](pygraph& zz) { return zz.EDGES; },
                               "The adjacency list of the graph (Readonly)")
        .def("get_max_clique", &pygraph::get_max_clique,
             p::call_guard<p::scoped_ostream_redirect, p::scoped_estream_redirect>(),
             return_value_policy::copy)
        .def("continue_search", &pygraph::continue_search)
        .def("reset_search", &pygraph::reset_search)
        .def("__repr__", [](pygraph& zz) { return zz.showdata(); })
        .def("__str__", [](pygraph& zz) { return zz.showdata(); });
}
