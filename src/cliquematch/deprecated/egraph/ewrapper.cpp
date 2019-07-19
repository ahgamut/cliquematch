#include <wrappy/egraph/egraph.h>

namespace py = pybind11;
using namespace pybind11;

void init_egraph(py::module& m) {
    class_<Egraph, graph2>(m, "Egraph",
                           R"cmbase(
        Create a correspondence graph from two sets of points.
        **Note:** (cpp_subclass) the sets of points cannot be accessed once loaded.

        Attributes
        ----------
        epsilon : float
            The error bound for edge construction.
            Refer the paper for details.

        )cmbase")
        .def(init<Eigen::Ref<matrix>, u32, Eigen::Ref<matrix>, u32>(),
             "pts1"_a.noconvert(), "pts1_len"_a, "pts2"_a.noconvert(),
             "pts2_len"_a,
             R"cmbase(
	    Initialize a correspondence graph from 2 numpy arrays
        Distance function is taken as Euclidean distance

	    Parameters
	    ----------

	    pts1 : `Nx2` `numpy` array of points

	    pts2 : `Nx2` `numpy` array of points

	    )cmbase")
        .def(init<Eigen::Ref<matrix>, u32, Eigen::Ref<matrix>, u32,
                  std::function<double(Eigen::Ref<matrix>&, u32, u32)>, bool>(),
             R"cmbase(
                Initialize a correspondence graph from 2 numpy arrays along with a custom distance function for the first one
                )cmbase")
        .def(init<Eigen::Ref<matrix>, u32, Eigen::Ref<matrix>, u32,
                  std::function<double(Eigen::Ref<matrix>&, u32, u32)>,
                  std::function<double(Eigen::Ref<matrix>&, u32, u32)>, bool,
                  bool>(),
             R"cmbase(
                Initialize a correspondence graph from 2 numpy arrays along with their custom distance functions
                )cmbase")
        .def_readwrite("epsilon", &Egraph::pts_epsilon,
                       "Set error bound for edge construction")
        .def("build_edges", &Egraph::build_edges,
             R"cmbase(
	    Build the edges of the correspondence graph.

	    Warnings
	    --------
	        1. This method must be called every time the value of `epsilon` is changed, before a clique is computed.
	        2. A large value of `epsilon` can lead to a highly dense correspondence graph being constructed,
	        which will take a lot of time (possibly longer than finding a large clique in the graph!).
	    )cmbase")
        .def("build_edges_with_condition", &Egraph::build_edges_with_condition,
             "pts1"_a.noconvert(), "pts2"_a.noconvert(), "condition_func"_a,
             "use_condition_only"_a = false,
             R"cmbase(
	    Build the edges of the correspondence graph along with a custom condition to check every time one is considering a mapping.

	    Warnings
	    --------
	        1. This method must be called every time the value of `epsilon` is changed, before a clique is computed.
	        2. A large value of `epsilon` can lead to a highly dense correspondence graph being constructed,
	        which will take a lot of time (possibly longer than finding a large clique in the graph!).
	    )cmbase")
        .def("get_correspondence", &Egraph::get_correspondence,
             R"cmbase(
	    Get the indices of corresponding points of the two sets
	    This method calls `get_max_clique()` and then finds the indices from the result.

	    Returns
	    -------
	    list(ndarray, ndarray)
	        The first element of the list contains the indices from the first pointset,
	        The second element contains the indices from the second pointset.
	    )cmbase")
        .def("__repr__", [](Egraph& zz) { return zz.showdata(true); })
        .def("__str__", [](Egraph& zz) { return zz.showdata(false); });
}
