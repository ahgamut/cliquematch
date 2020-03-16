#ifndef WRAP_TEMPLATE_HPP
#define WRAP_TEMPLATE_HPP

#include <templates/ext_template.h>

namespace py = pybind11;

template <typename List1, typename Delta1, typename List2, typename Delta2,
	  typename EpsType>
void init_GraphTemplate(py::module& m, std::string classname) {
    using namespace pybind11;
    using GClass = GraphTemplate<List1, Delta1, List2, Delta2, EpsType>;
    class_<GClass, pygraph>(m, classname.c_str())
	.def(init<List1&, std::size_t, List2&, std::size_t>(), "set1"_a.noconvert(),
	     "set1_len"_a, "set2"_a.noconvert(), "set2_len"_a)
	.def(init<List1&, std::size_t, List2&, std::size_t,
		  std::function<Delta1(List1&, std::size_t, std::size_t)>, bool>(),
	     "set1"_a.noconvert(), "set1_len"_a, "set2"_a.noconvert(),
	     "set2_len"_a, "dist_set1"_a, "is_d1_symmetric"_a)
	.def(init<List1&, std::size_t, List2&, std::size_t,
		  std::function<Delta1(List1&, std::size_t, std::size_t)>, bool,
		  std::function<Delta2(List2&, std::size_t, std::size_t)>, bool>(),
	     "set1"_a.noconvert(), "set1_len"_a, "set2"_a.noconvert(),
	     "set2_len"_a, "dist_set1"_a, "is_d1_symmetric"_a, "dist_set2"_a,
	     "is_d2_symmetric"_a)
	.def_readwrite("epsilon", &GClass::pts_epsilon,
		       "Set error bound for edge construction using d1 and d2")
	.def("build_edges", &GClass::build_edges, "set1"_a.noconvert(),
	     "set2"_a.noconvert())
	.def("build_edges_with_condition", &GClass::build_edges_with_condition,
	     "set1"_a, "set2"_a, "condition_func"_a, "use_cfunc_only"_a = false)
	.def("get_correspondence", &GClass::get_correspondence,
	     py::return_value_policy::copy)
	.def("__repr__", [](GClass& zz) { return zz.showdata(); })
	.def("__str__", [](GClass& zz) { return zz.showdata(); });
}

#endif /* WRAP_TEMPLATE_HPP */

