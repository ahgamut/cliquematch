#ifndef A2AGRAPH_H
#define A2AGRAPH_H

#include <pybind11/eigen.h>
#include <ext/eigen_distance.hpp>
#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

namespace cliquematch
{
namespace ext
{
    template <>
    double dummy_comparison<Eigen::Ref<DoubleMatrixR>, double>(
        Eigen::Ref<DoubleMatrixR>& ll, std::size_t i, std::size_t j);

    // Implemented in A2AGraph.cpp
    extern template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

    extern template class GraphTemplate<Eigen::Ref<DoubleMatrixR>,
                                        Eigen::Ref<DoubleMatrixR>>;
    using A2AGraph =
        GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;

}  // namespace ext
}  // namespace cliquematch
#endif /* A2AGRAPH_H */

