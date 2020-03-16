#ifndef A2LGRAPH_H
#define A2LGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In A2Agraph.cpp

extern template struct relset<Eigen::Ref<matrix>, double>;

// In L2Lgraph.cpp

extern template struct relset<py::list, double>;

// In A2LGraph.cpp

extern template std::vector<std::set<std::size_t> >
edges_from_relsets<Eigen::Ref<matrix>, double, py::list, double, double>(
    std::size_t&, std::size_t&, relset<Eigen::Ref<matrix>, double>&,
    relset<py::list, double>&, const double);

extern template std::vector<std::set<std::size_t> >
efr_condition<Eigen::Ref<matrix>, double, py::list, double, double>(
    std::size_t&, std::size_t&, relset<Eigen::Ref<matrix>, double>&,
    relset<py::list, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

template <>
GraphTemplate<Eigen::Ref<matrix>, double, py::list, double, double>::GraphTemplate(
    Eigen::Ref<matrix>& pts1, std::size_t pts1_len, py::list& pts2,
    std::size_t pts2_len);

extern template struct GraphTemplate<Eigen::Ref<matrix>, double, py::list, double,
                                     double>;

#endif /* A2LGRAPH_H */

