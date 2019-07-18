#include <cm_base/core/pygraph.h>
#include <pybind11/iostream.h>
#include <pybind11/stl.h>

namespace p = pybind11;
using namespace pybind11;

void init_pygraph(p::module& m) {
    class_<pygraph>(m, "Graph",
                   R"cmbase(
        Class to load/operate on/store a large sparse graph.

        Attributes
        ----------
        use_heuristic : bool
            If `True`, uses a heuristic to find a large clique (not necessarily maximum) in polynomial time.
        use_dfs : bool
            If `True` performs the depth_first search to search for cliques
        size_limit : int
            Set the upper bound on the size of clique to find. `get_max_clique()`
            will terminate if a clique of size `>= size_limit` is found.
        time_limit : float
            Set the upper bound on the time spent in a search. `get_max_clique()`
            returns the largest clique found at the time of termination.
        n_vertices : int, readonly
            The number of vertices in the graph.
        n_edges : int, readonly
            The number of edges in the graph.
        current_vertex : int, readonly
            The vertex that will be searched at the start of the next search.
            (`\implies` all vertices before this have been searched for cliques).
        search_done : bool, readonly
            `True` if all vertices in the graph have been searched for cliques.

        )cmbase")
        .def(init<std::string, unsigned int>(), arg("filename"), arg("type"),
             R"cmbase(
        Initialize a graph from a .mtx file

        Parameters
        ----------

        filename : str
            Path to the `.mtx` (or any text in a similar format) file containing the sparse graph
        type : int {1, 2}
            Format of the sparse graph in the text file.
            `type=1` implies a format similar to `ca-AstroPh.mtx <https://sparse.tamu.edu/SNAP/ca-AstroPh>`
            `type=2` implies a format similar to `cond-mat-2003.mtx <https://sparse.tamu.edu/Newman/cond-mat-2003>` (Note that edge weights are discarded)

        )cmbase")
        .def(init<p::array_t<bool> >(), arg("adjmat"),
             R"cmbase(
        Initialize a graph from an adjacency matrix

        Parameters:
        -----------

        adjmat : 2D matrix of `numpy.bool`
            A `NxN` adjacency matrix of the graph.
        )cmbase")
        .def(init<p::array_t<unsigned int>, unsigned int>(), arg("edge_list"),
             arg("n_vertices"),
             R"cmbase(
            Initialize a graph from a list of edges

            Parameters
            ----------

            edgelist : `Nx2` array of `numpy.uint32`
                The list of edges in the graph, each edge is dscribed by the pair of vertices it connects
            n_vertices : int
                The total number of vertices in the graph
            )cmbase")
        .def_readwrite("use_heuristic", &pygraph::use_heur,
                       "Search using the heuristic if true")
        .def_readwrite("use_dfs", &pygraph::use_dfs,
                       "Perform the depth-first search if true")
        .def_readwrite("size_limit", &pygraph::clique_lim,
                       "Set the limit on the size of clique to find")
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
        .def_readonly("adjacency_list", &pygraph::EDGES,
                      "The adjacency list of the graph (Readonly)")
        .def("get_max_clique", &pygraph::get_max_clique,
             p::call_guard<p::scoped_ostream_redirect,
                           p::scoped_estream_redirect>(),
             return_value_policy::copy,
             R"cmbase(
	    Get a maximum clique in the graph.

	    Returns
	    -------
        List of unsigned ints
            Returns the vertices of the largest clique found so far.

	    )cmbase")
        .def("continue_search", &pygraph::continue_search,
             R"cmbase(
	    Resume the search for a maximum clique at `current_vertex`.
	    Does nothing if all the vertices have been searched.

	    )cmbase")
        .def("__repr__", [](pygraph& zz) { return zz.showdata(); })
        .def("__str__", [](pygraph& zz) { return zz.showdata(); });
}
