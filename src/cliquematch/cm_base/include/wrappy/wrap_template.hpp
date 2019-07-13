#ifndef WRAP_TEMPLATE_HPP
#define WRAP_TEMPLATE_HPP

#include <cm_base/include/wrappy/ext_template.h>

namespace py = pybind11;

template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
void init_GraphTemplate(py::module& m, std::string classname) {
    using namespace pybind11;
    using GClass = GraphTemplate<List1, Delta1, List2, Delta2, EpsType>;
    class_<GClass, graph2>(m, classname.c_str(),
                           R"cmbase(
            Correspondence graph class for two sets S1 and S2.
            (cpp_subclass) elements of the sets are not accessible

            Attributes
            ----------
            epsilon:
                The error bound for edge construction.
                Refer to paper for details.

            )cmbase")
        .def(init<List1&, u32, List2&, u32>(), "set1"_a.noconvert(),
             "set1_len"_a, "set2"_a.noconvert(), "set2_len"_a,
             R"cmbase(
            Initialize the correspondence graph from the two given sets and their corresponding number of elements.
            Distance function is taken as the provided default.

            )cmbase")
        .def(init<List1&, u32, List2&, u32,
                  std::function<Delta1(List1&, u32, u32)>, bool>(),
             "set1"_a.noconvert(), "set1_len"_a, "set2"_a.noconvert(),
             "set2_len"_a, "dist_set1"_a, "is_d1_symmetric"_a,
             R"cmbase(
                    Initialize the correspondence graph with the two given sets, their corresponding number of elements, a custom distance function for set S1, and the default for set S2.
            )cmbase")
        .def(init<List1&, u32, List2&, u32,
                  std::function<Delta1(List1&, u32, u32)>, bool,
                  std::function<Delta2(List2&, u32, u32)>, bool>(),
             "set1"_a.noconvert(), "set1_len"_a, "set2"_a.noconvert(),
             "set2_len"_a, "dist_set1"_a, "is_d1_symmetric"_a, "dist_set2"_a,
             "is_d2_symmetric"_a,
             R"cmbase(
                    Initialize the correspondence graph with the two given sets, their corresponding number of elements, and custom distance functions for both sets S1, and S2.
            )cmbase")
        .def_readwrite("epsilon", &GClass::pts_epsilon,
                       "Set error bound for edge construction using d1 and d2")
        .def("build_edges", &GClass::build_edges, "set1"_a.noconvert(),
             "set2"_a.noconvert(),
             R"cmbase(
	    Build the edges of the correspondence graph.

	    Warnings
	    --------
	        1. This method must be called every time the value of `epsilon` is changed, before a clique is computed.
	        2. A large value of `epsilon` can lead to a highly dense correspondence graph being constructed,
	        which will take a lot of time (possibly longer than finding a large clique in the graph!).
	        )cmbase")
        .def("build_edges_with_condition", &GClass::build_edges_with_condition,
             "set1"_a, "set2"_a, "condition_func"_a, "use_cfunc_only"_a = false,
             R"cmbase(
	    Build the edges of the correspondence graph along with a custom condition to check every time one is considering a mapping.

	    Warnings
	    --------
	        1. This method must be called every time the value of `epsilon` is changed, before a clique is computed.
	        2. A large value of `epsilon` can lead to a highly dense correspondence graph being constructed,
	        which will take a lot of time (possibly longer than finding a large clique in the graph!).
	        )cmbase")
        .def("get_correspondence", &GClass::get_correspondence,
             py::return_value_policy::copy,
             R"cmbase(
	    Get the indices of corresponding points of the two sets
	    This method calls `get_max_clique()` and then finds the indices from the result.

	    Returns
	    -------
	    List(List, List)
	        The first element of the list contains the indices from the first pointset,
	        The second element contains the indices from the second pointset.
    	    )cmbase")
        .def("__repr__", [](GClass& zz) { return zz.showdata(); })
        .def("__str__", [](GClass& zz) { return zz.showdata(); });
}

#endif /* WRAP_TEMPLATE_HPP */

