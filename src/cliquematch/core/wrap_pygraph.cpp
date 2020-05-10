#include <core/pygraph.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

namespace p = pybind11;
using namespace pybind11;

void init_pygraph(p::module& m)
{
    class_<pygraph>(m, "Graph")
        .def(init<>())
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
        .def("get_max_clique", &pygraph::get_max_clique,
             p::call_guard<p::scoped_ostream_redirect, p::scoped_estream_redirect>(),
             return_value_policy::copy)
        .def("continue_search", &pygraph::continue_search)
        .def("reset_search", &pygraph::reset_search)
        .def_static("from_file", &from_file, arg("filename"), return_value_policy::move)
        .def_static("from_edgelist", &from_edgelist, arg("edgelist"),
                    arg("num_vertices"), return_value_policy::move)
        .def_static("from_matrix", &from_adj_matrix, arg("adjmat"),
                    return_value_policy::move)
        .def_static("from_adjlist", &from_adj_list, arg("num_vertices"),
                    arg("num_edges"), arg("edges"), return_value_policy::move)
        .def("to_file", &pygraph::to_file, arg("filename"))
        .def("to_edgelist", &pygraph::to_edgelist)
        .def("to_matrix", &pygraph::to_adj_matrix)
        .def("to_adjlist", &pygraph::to_adj_list)
        .def("__repr__", [](pygraph& zz) { return zz.showdata(); })
        .def("__str__", [](pygraph& zz) { return zz.showdata(); });
}
