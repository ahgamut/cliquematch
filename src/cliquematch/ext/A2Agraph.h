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
        const Eigen::Ref<DoubleMatrixR>& ll, const u64 i, const u64 j);

    // Implemented in A2AGraph.cpp
    extern template struct relset<Eigen::Ref<DoubleMatrixR>, double>;
    extern template bool build_edges(
        pygraph& pg, const Eigen::Ref<DoubleMatrixR>& pts1, const u64 pts1_len,
        const Eigen::Ref<DoubleMatrixR>& pts2, const u64 pts2_len, const double epsilon,
        const std::function<bool(const Eigen::Ref<DoubleMatrixR>&, const u64, const u64,
                                 const Eigen::Ref<DoubleMatrixR>&, const u64,
                                 const u64)>
            cfunc,
        const std::function<double(const Eigen::Ref<DoubleMatrixR>&, const u64,
                                   const u64)>
            d1,
        const bool is_d1_symmetric,
        const std::function<double(const Eigen::Ref<DoubleMatrixR>&, const u64,
                                   const u64)>
            d2,
        const bool is_d2_symmetric);

}  // namespace ext
}  // namespace cliquematch
#endif /* A2AGRAPH_H */
