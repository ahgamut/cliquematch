#ifndef A2AGRAPH_H
#define A2AGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// Implemented in A2AGraph.cpp

extern template struct relset<Eigen::Ref<matrix>, double>;

extern template std::vector<std::set<std::size_t> >
edges_from_relsets<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<matrix>, double>&,
    const relset<Eigen::Ref<matrix>, double>&, const double);

extern template std::vector<std::set<std::size_t> >
efr_condition<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<matrix>, double>&,
    const relset<Eigen::Ref<matrix>, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

// specializing the template for the Eigen case as L2 Norm
template <>
GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>::
    GraphTemplate(
        Eigen::Ref<matrix>& pts1, std::size_t pts1_len, Eigen::Ref<matrix>& pts2,
        std::size_t pts2_len,
        std::function<double(Eigen::Ref<matrix>&, std::size_t, std::size_t)> d1,
        bool is_d1_symmetric);
template <>
GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double,
              double>::GraphTemplate(Eigen::Ref<matrix>& pts1, std::size_t pts1_len,
                                     Eigen::Ref<matrix>& pts2, std::size_t pts2_len);

extern template struct GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>,
                                     double, double>;

#endif /* A2AGRAPH_H */

