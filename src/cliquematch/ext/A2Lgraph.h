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

extern template std::vector<std::set<u32> >
edges_from_relsets<Eigen::Ref<matrix>, double, py::list, double, double>(
    u32&, u32&, relset<Eigen::Ref<matrix>, double>&, relset<py::list, double>&,
    const double);

extern template std::vector<std::set<u32> >
efr_condition<Eigen::Ref<matrix>, double, py::list, double, double>(
    u32&, u32&, relset<Eigen::Ref<matrix>, double>&, relset<py::list, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

template <>
GraphTemplate<Eigen::Ref<matrix>, double, py::list, double,
              double>::GraphTemplate(Eigen::Ref<matrix>& pts1, u32 pts1_len,
                                     py::list& pts2, u32 pts2_len);

extern template struct GraphTemplate<Eigen::Ref<matrix>, double, py::list,
                                     double, double>;

#endif /* A2LGRAPH_H */

