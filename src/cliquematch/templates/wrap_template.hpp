#ifndef WRAP_TEMPLATE_HPP
#define WRAP_TEMPLATE_HPP

#include <templates/ext_template.h>
namespace cliquematch
{
namespace ext
{
    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    void init_GraphTemplate(py::module& m)
    {
        using namespace pybind11;

        m.def("_build_edges_metric_only",
              [](pygraph& pg, const List1& l1, const std::size_t l1_len,
                 const List2& l2, const std::size_t l2_len, const EpsType eps) {
                  return build_edges_metric_only(pg, l1, l1_len, l2, l2_len, eps);
              },
              "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
              "len2"_a, "epsilon"_a);
        m.def(
            "_build_edges_metric_only",
            [](pygraph& pg, const List1& l1, const std::size_t l1_len, const List2& l2,
               const std::size_t l2_len, const EpsType eps,
               std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
                   d1,
               const bool is_d1_symmetric) {
                return build_edges_metric_only(pg, l1, l1_len, l2, l2_len, eps, d1,
                                               is_d1_symmetric);
            },
            "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
            "len2"_a, "epsilon"_a, "d1"_a, "is_d1_symmetric"_a);
        m.def("_build_edges_metric_only",
              &build_edges_metric_only<List1, List2, Delta1, Delta2, EpsType>,
              "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
              "len2"_a, "epsilon"_a, "d1"_a, "is_d1_symmetric"_a, "d2"_a,
              "is_d2_symmetric"_a);
        m.def("_build_edges_condition_only",
              &build_edges_condition_only<List1, List2, Delta1, Delta2, EpsType>,
              "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
              "len2"_a, "condition_func"_a);

        m.def("_build_edges", &build_edges<List1, List2, Delta1, Delta2, EpsType>,
              "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
              "len2"_a, "epsilon"_a, "condition_func"_a, "d1"_a, "is_d1_symmetric"_a,
              "d2"_a, "is_d2_symmetric"_a);
        m.def(
            "_build_edges",
            [](pygraph& pg, const List1& l1, const std::size_t l1_len, const List2& l2,
               const std::size_t l2_len, const EpsType eps,
               const std::function<bool(const List1&, const std::size_t,
                                        const std::size_t, const List2&,
                                        const std::size_t, const std::size_t)>
                   cfunc,
               std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
                   d1,
               bool is_d1_symmetric) {
                return build_edges(pg, l1, l1_len, l2, l2_len, eps, cfunc, d1,
                                   is_d1_symmetric);
            },
            "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
            "len2"_a, "epsilon"_a, "condition_func"_a, "d1"_a, "is_d1_symmetric"_a);
        m.def("_build_edges",
              [](pygraph& pg, const List1& l1, const std::size_t l1_len,
                 const List2& l2, const std::size_t l2_len, const EpsType eps,
                 const std::function<bool(const List1&, const std::size_t,
                                          const std::size_t, const List2&,
                                          const std::size_t, const std::size_t)>
                     cfunc) {
                  return build_edges(pg, l1, l1_len, l2, l2_len, eps, cfunc);
              },
              "G"_a.none(false), "set1"_a.noconvert(), "len1"_a, "set2"_a.noconvert(),
              "len2"_a, "epsilon"_a, "condition_func"_a);
    }
}  // namespace ext
}  // namespace cliquematch

#endif /* WRAP_TEMPLATE_HPP */

