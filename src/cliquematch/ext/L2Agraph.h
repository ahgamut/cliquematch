#ifndef L2AGRAPH_H
#define L2AGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In A2Agraph.cpp

extern template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

// In L2Lgraph.cpp

extern template struct relset<py::list, double>;

// In L2AGraph.cpp

extern template std::vector<std::set<std::size_t> >
edges_from_relsets<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<Eigen::Ref<DoubleMatrixR>, double>&, const double);

extern template std::vector<std::set<std::size_t> >
efr_condition<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<Eigen::Ref<DoubleMatrixR>, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

// Specializing ctors to account for euclidean default
template <>
GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>::
    GraphTemplate(py::list& pts1, std::size_t pts1_len, Eigen::Ref<DoubleMatrixR>& pts2,
                  std::size_t pts2_len,
                  std::function<double(py::list&, std::size_t, std::size_t)> d1,
                  bool is_d1_symmetric);

template <>
GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double,
              double>::GraphTemplate(py::list& pts1, std::size_t pts1_len,
                                     Eigen::Ref<DoubleMatrixR>& pts2,
                                     std::size_t pts2_len);

extern template struct GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>,
                                     double, double>;

#endif /* L2AGRAPH_H */

