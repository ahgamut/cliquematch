#ifndef A2LGRAPH_H
#define A2LGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In A2Agraph.cpp

extern template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

// In L2Lgraph.cpp

extern template struct relset<py::list, double>;

// In A2LGraph.cpp

template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list, double>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len, py::list& pts2,
    std::size_t pts2_len);

extern template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list, double>;

#endif /* A2LGRAPH_H */

