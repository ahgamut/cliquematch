#ifndef WRAP_TEMPLATE_HPP
#define WRAP_TEMPLATE_HPP

#include <templates/ext_template.h>

namespace py = pybind11;

template <typename List1, typename List2, typename Delta1 = double,
          typename Delta2 = Delta1, typename EpsType = Delta1>
bool build_edges_wrapped(
    GraphTemplate<List1, List2, Delta1, Delta2, EpsType>& zz, List1& set1, size_t len1,
    List2& set2, size_t len2,
    std::function<bool(List1&, size_t, size_t, List2&, size_t, size_t)> cfunc,
    bool use_cfunc_only, std::function<Delta1(List1&, size_t, size_t)> d1,
    std::function<Delta2(List2&, size_t, size_t)> d2)
{
    if (d1)
    {
        if (d2)
        {
            return zz.build_edges(set1, len1, set2, len2, cfunc, use_cfunc_only, d1,
                                  d2);
        }
        else
        {
            py::print("d2 is None");
            return zz.build_edges(set1, len1, set2, len2, cfunc, use_cfunc_only, d1);
        }
    }
    else
    {
        py::print("d1 is None");
        return zz.build_edges(set1, len1, set2, len2, cfunc, use_cfunc_only);
    }
    return true;
}

template <typename List1, typename List2, typename Delta1 = double,
          typename Delta2 = Delta1, typename EpsType = Delta1>
void init_GraphTemplate(py::module& m, std::string classname)
{
    using namespace pybind11;
    using GClass = GraphTemplate<List1, List2, Delta1, Delta2, EpsType>;
    class_<GClass, pygraph>(m, classname.c_str())
        .def(init<>())
        .def_readwrite("epsilon", &GClass::pts_epsilon,
                       "Set error bound for edge construction with metrics")
        .def("build_edges_metric_only",
             py::overload_cast<List1&, size_t, List2&, size_t>(
                 &GClass::build_edges_metric_only),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a)
        .def("build_edges_metric_only",
             py::overload_cast<List1&, size_t, List2&, size_t,
                               std::function<Delta1(List1&, size_t, size_t)>, bool>(
                 &GClass::build_edges_metric_only),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a, "d1"_a,
             "is_d1_symmetric"_a)
        .def("build_edges_metric_only",
             py::overload_cast<List1&, size_t, List2&, size_t,
                               std::function<Delta1(List1&, size_t, size_t)>, bool,
                               std::function<Delta2(List2&, size_t, size_t)>, bool>(
                 &GClass::build_edges_metric_only),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a, "d1"_a,
             "is_d1_symmetric"_a, "d2"_a, "is_d2_symmetric"_a)
        .def("build_edges_condition_only", &GClass::build_edges_condition_only,
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
             "condition_func"_a)
        .def("build_edges",
             py::overload_cast<
                 List1&, size_t, List2&, size_t,
                 std::function<bool(List1&, size_t, size_t, List2&, size_t, size_t)>,
                 std::function<Delta1(List1&, size_t, size_t)>, bool,
                 std::function<Delta2(List2&, size_t, size_t)>, bool>(
                 &GClass::build_edges),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
             "condition_func"_a, "d1"_a, "is_d1_symmetric"_a, "d2"_a,
             "is_d2_symmetric"_a)
        .def("build_edges",
             py::overload_cast<
                 List1&, size_t, List2&, size_t,
                 std::function<bool(List1&, size_t, size_t, List2&, size_t, size_t)>,
                 std::function<Delta1(List1&, size_t, size_t)>, bool>(
                 &GClass::build_edges),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
             "condition_func"_a, "d1"_a, "is_d1_symmetric"_a)
        .def("build_edges",
             py::overload_cast<
                 List1&, size_t, List2&, size_t,
                 std::function<bool(List1&, size_t, size_t, List2&, size_t, size_t)>>(
                 &GClass::build_edges),
             "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
             "condition_func"_a)

        .def("get_correspondence", &GClass::get_correspondence)
        .def("__repr__", [](GClass& zz) { return zz.showdata(); })
        .def("__str__", [](GClass& zz) { return zz.showdata(); });
}

#endif /* WRAP_TEMPLATE_HPP */

