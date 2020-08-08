#include <core/pygraph.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

namespace cliquematch
{
void init_pygraph(pybind11::module& m)
{
    namespace py = pybind11;
    namespace cm_core = cliquematch::core;
    using namespace pybind11;
    using cliquematch::core::pygraph;

    class_<pygraph, std::unique_ptr<pygraph, cm_core::pygraphDeleter>>(m, "Graph")
        .def(py::init<>())
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
             "Finds a maximum clique in graph within the given bounds",
             py::call_guard<py::scoped_ostream_redirect, py::scoped_estream_redirect>(),
             py::return_value_policy::copy)
        .def("continue_search", &pygraph::continue_search,
             "Continue the clique search if the entire graph has not been searched")
        .def("reset_search", &pygraph::reset_search,
             "Reset the search for maximum cliques")
        .def_static("from_file", &cm_core::from_file,
                    "Constructs `Graph` instance from reading a Matrix Market file",
                    arg("filename"), py::return_value_policy::move)
        .def_static("from_edgelist", &cm_core::from_edgelist,
                    "Constructs `Graph` instance from the given edge list",
                    arg("edgelist"), arg("num_vertices"), py::return_value_policy::move)
        .def_static("from_matrix", &cm_core::from_adj_matrix,
                    "Constructs `Graph` instance from the given adjacency matrix",
                    arg("adjmat"), py::return_value_policy::move)
        .def_static("from_adjlist", &cm_core::from_adj_list,
                    "Constructs `Graph` instance from the given adjacency list",
                    arg("num_vertices"), arg("num_edges"), arg("edges"),
                    py::return_value_policy::move)
        .def("to_file", &pygraph::to_file,
             "Exports `Graph` instance to a Matrix Market file", arg("filename"))
        .def("to_edgelist", &pygraph::to_edgelist,
             "Exports `Graph` instance to an edge list")
        .def("to_matrix", &pygraph::to_adj_matrix,
             "Exports `Graph` instance to a boolean matrix")
        .def("to_adjlist", &pygraph::to_adj_list,
             "Exports `Graph` instance to an adjacency list")
        .def("__repr__", &pygraph::showdata)
        .def("__str__", &pygraph::showdata);
}
}  // namespace cliquematch
