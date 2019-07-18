#ifndef L2AGRAPH_H
#define L2AGRAPH_H

#include <pybind11/eigen.h>
#include <cm_base/ext/eigen_distance.hpp>
#include <cm_base/templates/ext_template.hpp>   // contains only templates
#include <cm_base/templates/wrap_template.hpp>  // contains only templates

// In A2Agraph.cpp

extern template struct relset<Eigen::Ref<matrix>, double>;

// In L2Lgraph.cpp

extern template struct relset<py::list, double>;

// In L2AGraph.cpp

extern template std::vector<std::set<u32> >
edges_from_relsets<py::list, double, Eigen::Ref<matrix>, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<Eigen::Ref<matrix>, double>&,
    const double);

extern template std::vector<std::set<u32> >
efr_condition<py::list, double, Eigen::Ref<matrix>, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<Eigen::Ref<matrix>, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

// Specializing ctors to account for euclidean default
template <>
GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>::
    GraphTemplate(py::list& pts1, u32 pts1_len, Eigen::Ref<matrix>& pts2,
                  u32 pts2_len, std::function<double(py::list&, u32, u32)> d1,
                  bool is_d1_symmetric);

template <>
GraphTemplate<py::list, double, Eigen::Ref<matrix>, double,
              double>::GraphTemplate(py::list& pts1, u32 pts1_len,
                                     Eigen::Ref<matrix>& pts2, u32 pts2_len);

extern template struct GraphTemplate<py::list, double, Eigen::Ref<matrix>,
                                     double, double>;

#endif /* L2AGRAPH_H */

