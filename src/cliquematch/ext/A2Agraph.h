#ifndef A2AGRAPH_H
#define A2AGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// Implemented in A2AGraph.cpp

extern template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

// specializing the template for the Eigen case as L2 Norm
template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len,
    Eigen::Ref<DoubleMatrixR>& pts2, std::size_t pts2_len,
    std::function<double(Eigen::Ref<DoubleMatrixR>&, std::size_t, std::size_t)> d1,
    bool is_d1_symmetric);
template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len,
    Eigen::Ref<DoubleMatrixR>& pts2, std::size_t pts2_len);

extern template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>,
                                     Eigen::Ref<DoubleMatrixR>>;

#endif /* A2AGRAPH_H */

