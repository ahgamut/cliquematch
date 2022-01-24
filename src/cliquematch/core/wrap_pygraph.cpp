#include <core/pygraph.h>
#include <core/pynwgraph.h>
#include <core/pyiterator.h>
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
    using cliquematch::core::pynwgraph;

    class_<pygraph>(m, "Graph")
        .def(py::init<>())
        .def_readonly("search_done", &pygraph::finished_all,
                      "Whether the search has been completed (Readonly)")
        .def_readonly("n_vertices", &pygraph::nvert,
                      "Number of vertices in the graph (Readonly)")
        .def_readonly("n_edges", &pygraph::nedges,
                      "Number of edges in the graph (Readonly)")
        .def("_vertex_neighbors", &pygraph::get_vertex_data,
             "Return the neighbors of the given vertex", "v"_a)
        .def("get_max_clique", &pygraph::get_max_clique,
             "Find a maximum clique in graph within the given bounds",
             "lower_bound"_a = 1, "upper_bound"_a = 0xFFFF, "time_limit"_a = 0.0,
             "use_heuristic"_a = true, "use_dfs"_a = true, "continue_search"_a = false)
        .def("reset_search", &pygraph::reset_search,
             "Reset the clique search to try with different parameters")
        .def("_get_correspondence", &pygraph::get_correspondence, "len1"_a, "len2"_a,
             "lower_bound"_a = 1, "upper_bound"_a = 0xFFFF, "time_limit"_a = 0.0,
             "use_heuristic"_a = true, "use_dfs"_a = true, "continue_search"_a = false)
        .def(
            "all_cliques",
            [](py::object s, u64 clique_size)
            { return cm_core::CliqueIterator(s.cast<pygraph&>(), s, clique_size); },
            "size"_a)
        .def(
            "_all_correspondences",
            [](py::object s, u64 len1, u64 len2, u64 clique_size)
            {
                return cm_core::CorrespondenceIterator(s.cast<pygraph&>(), s, len1,
                                                       len2, clique_size);
            },
            "len1"_a, "len2"_a, "size"_a)
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

    class_<pynwgraph>(m, "NWGraph")
        .def(py::init<>())
        .def_readonly("search_done", &pynwgraph::finished_all,
                      "Whether the search has been completed (Readonly)")
        .def_readonly("n_vertices", &pynwgraph::nvert,
                      "Number of vertices in the graph (Readonly)")
        .def_readonly("n_edges", &pynwgraph::nedges,
                      "Number of edges in the graph (Readonly)")
        .def("get_max_clique", &pynwgraph::get_max_clique,
             "Find a maximum clique in graph within the given bounds",
             "lower_bound"_a = 1, "upper_bound"_a = 0xFFFF, "use_heuristic"_a = true,
             "use_dfs"_a = true)
        .def("get_clique_weight", &pynwgraph::get_clique_weight,
             "calculate the weight of the given clique in this graph", "clique"_a)
        .def("reset_search", &pynwgraph::reset_search,
             "Reset the clique search to try with different parameters")
        .def("_get_vertex_weights", &pynwgraph::get_all_weights,
             "return the weights of all the vertices")
        .def("_vertex_neighbors", &pynwgraph::get_vertex_data,
             "Return the neighbors of the given vertex", "v"_a)
        .def("_get_correspondence", &pynwgraph::get_correspondence, "len1"_a, "len2"_a,
             "lower_bound"_a = 1, "upper_bound"_a = 0xFFFF, "use_heuristic"_a = true,
             "use_dfs"_a = true)
        .def(
            "all_cliques",
            [](py::object s, double clique_size)
            { return cm_core::NWCliqueIterator(s.cast<pynwgraph&>(), s, clique_size); },
            "size"_a)
        .def(
            "_all_correspondences",
            [](py::object s, u64 len1, u64 len2, double clique_size)
            {
                return cm_core::NWCorrespondenceIterator(s.cast<pynwgraph&>(), s, len1,
                                                         len2, clique_size);
            },
            "len1"_a, "len2"_a, "size"_a)
        .def_static(
            "from_edgelist", &cm_core::nw_from_edgelist,
            "Constructs `NWGraph` instance from the given edge list and vertex weights",
            arg("edgelist"), arg("num_vertices"), arg("weights"),
            py::return_value_policy::move)
        .def_static("from_matrix", &cm_core::nw_from_adj_matrix,
                    "Constructs `NWGraph` instance from the given adjacency matrix and "
                    "vertex weights",
                    arg("adjmat"), arg("weights"), py::return_value_policy::move)
        .def_static("from_adjlist", &cm_core::nw_from_adj_list,
                    "Constructs `NWGraph` instance from the given adjacency list and "
                    "vertex weights",
                    arg("num_vertices"), arg("num_edges"), arg("edges"), arg("weights"),
                    py::return_value_policy::move)
        .def("to_edgelist", &pynwgraph::to_edgelist,
             "Exports `NWGraph` instance to an edge list")
        .def("to_matrix", &pynwgraph::to_adj_matrix,
             "Exports `NWGraph` instance to a boolean matrix")
        .def("to_adjlist", &pynwgraph::to_adj_list,
             "Exports `NWGraph` instance to an adjacency list")
        .def("__repr__", &pynwgraph::showdata)
        .def("__str__", &pynwgraph::showdata);
}
}  // namespace cliquematch
