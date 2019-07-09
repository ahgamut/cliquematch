#include <cm_base/wrappy/psgraph/psgraph.h>

namespace py = pybind11;
using namespace pybind11;

void init_psgraph(py::module& m) {
    class_<psgraph, graph2>(m, "psgraph", py::dynamic_attr(),
                            R"cmbase(
        Create a correspondence graph from two sets of points.
        **Note:** the sets of points cannot be accessed once loaded.

        Attributes
        ----------
        epsilon : float
            The error bound for edge construction.
            Refer the paper for details.

        bounds_choice : str
            Can be either `"constant"` or `"ratio"`.
            `"ratio"` is more lenient with deformations in the point-sets while finding correspondence.

        )cmbase")
        .def(init<std::string>(), arg("filename"),
             R"cmbase(
	    Initialize a correspondence graph from file

	    Parameters
	    ----------

	    filename : str
	        Path to the file containing the list of points.
	        The format must be as below:
	        First line gives the value of `epsilon`
	        Next line denotes `M`, the number of points in the first set
	        Next `M` lines contain one point each: `3.14 42`
	        Next line denotes `N` the number of points in the second set
	        Next `N` lines contain one point each: `1.618 2.728`

	    )cmbase")
        .def(init<Eigen::Ref<matrix>, Eigen::Ref<matrix>>(), arg("points1"),
             arg("points2"),
             R"cmbase(
	    Initialize a correspondence graph from 2 numpy arrays
        Array must be C-contiguous, `dtype=numpy.float64`
        Warning: deleting the arrays passed may lead to a crash

	    Parameters
	    ----------

	    points1 : `Nx2` `numpy` array of points

	    points2 : `Nx2` `numpy` array of points

	    )cmbase")
        .def_readwrite("epsilon", &psgraph::pts_epsilon,
                       "Set error bound for edge construction")
        .def_readwrite("bounds_choice", &psgraph::eps_choice,
                       "Set usage of error bound")
        .def("build_edges", &psgraph::build_edges,
             R"cmbase(
	    Build the edges of the correspondence graph.

	    Warnings
	    --------
	        1. This method must be called every time the value of `epsilon` is changed, before a clique is computed.
	        2. A large value of `epsilon` can lead to a highly dense correspondence graph being constructed,
	        which will take a lot of time (possibly longer than finding a large clique in the graph!).
	    )cmbase")
        .def("build_edges_via_steve_rule", &psgraph::build_edges_with_filter)
        .def("get_correspondence", &psgraph::get_correspondence,
             R"cmbase(
	    Get the corresponding points of the two point sets
	    This method calls `get_max_clique()` and then finds the required points from the result.

	    Returns
	    -------
	    list(ndarray, ndarray)
	        The first element of the list contains the correspondence points from the first pointset,
	        The second element contains the correspondence points from the second pointset.
	    )cmbase")
        .def("__repr__", [](psgraph& zz) { return zz.showdata(); })
        .def("__str__", [](psgraph& zz) { return zz.showdata(); });
}
