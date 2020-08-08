#ifndef WRAP_TEMPLATE_HPP
#define WRAP_TEMPLATE_HPP

#include <templates/ext_template.h>
namespace cliquematch
{
namespace ext
{
    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    void init_GraphTemplate(py::module& m, std::string classname)
    {
        using namespace pybind11;
        using GClass = GraphTemplate<List1, List2, Delta1, Delta2, EpsType>;
        class_<GClass, std::unique_ptr<GClass, pygraphDeleter>, pygraph>(
            m, classname.c_str(),
            "template subclass for correspondence graph construction")
            .def(init<>())
            .def_readwrite("epsilon", &GClass::pts_epsilon,
                           "Set error bound for edge construction with metrics")
            .def("_build_edges_metric_only",
                 (bool (GClass::*)(List1&, std::size_t, List2&, std::size_t))(
                     &GClass::build_edges_metric_only),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a)
            .def("_build_edges_metric_only",
                 (bool (GClass::*)(
                     List1&, std::size_t, List2&, std::size_t,
                     std::function<Delta1(List1&, std::size_t, std::size_t)>,
                     bool))(&GClass::build_edges_metric_only),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a, "d1"_a,
                 "is_d1_symmetric"_a)
            .def("_build_edges_metric_only",
                 (bool (GClass::*)(
                     List1&, std::size_t, List2&, std::size_t,
                     std::function<Delta1(List1&, std::size_t, std::size_t)>, bool,
                     std::function<Delta2(List2&, std::size_t, std::size_t)>,
                     bool))(&GClass::build_edges_metric_only),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a, "d1"_a,
                 "is_d1_symmetric"_a, "d2"_a, "is_d2_symmetric"_a)
            .def("_build_edges_condition_only", &GClass::build_edges_condition_only,
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
                 "condition_func"_a)
            .def("_build_edges",
                 (bool (GClass::*)(
                     List1&, std::size_t, List2&, std::size_t,
                     std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                        std::size_t, std::size_t)>,
                     std::function<Delta1(List1&, std::size_t, std::size_t)>, bool,
                     std::function<Delta2(List2&, std::size_t, std::size_t)>,
                     bool))(&GClass::build_edges),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
                 "condition_func"_a, "d1"_a, "is_d1_symmetric"_a, "d2"_a,
                 "is_d2_symmetric"_a)
            .def("_build_edges",
                 (bool (GClass::*)(
                     List1&, std::size_t, List2&, std::size_t,
                     std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                        std::size_t, std::size_t)>,
                     std::function<Delta1(List1&, std::size_t, std::size_t)>,
                     bool))(&GClass::build_edges),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
                 "condition_func"_a, "d1"_a, "is_d1_symmetric"_a)
            .def("_build_edges",
                 (bool (GClass::*)(
                     List1&, std::size_t, List2&, std::size_t,
                     std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                        std::size_t, std::size_t)>))(
                     &GClass::build_edges),
                 "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(), "len2"_a,
                 "condition_func"_a)

            .def("_get_correspondence", &GClass::get_correspondence)
            .def("__repr__", [](GClass& zz) { return zz.showdata(); })
            .def("__str__", [](GClass& zz) { return zz.showdata(); });
    }
}  // namespace ext
}  // namespace cliquematch

#endif /* WRAP_TEMPLATE_HPP */

