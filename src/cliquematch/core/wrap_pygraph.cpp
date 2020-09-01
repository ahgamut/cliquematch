#include <core/pygraph.h>
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

    class_<pygraph, std::unique_ptr<pygraph, cm_core::pygraphDeleter>>(m, "Graph")
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
        .def("all_cliques",
             [](py::object s, std::size_t clique_size) {
                 return cm_core::CliqueIterator(s.cast<pygraph&>(), s, clique_size);
             },
             "size"_a)
        .def("_all_correspondences",
             [](py::object s, std::size_t len1, std::size_t len2,
                std::size_t clique_size) {
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
}
}  // namespace cliquematch
