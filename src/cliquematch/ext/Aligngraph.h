#ifndef ALIGNGRAPH_H
#define ALIGNGRAPH_H

#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    bool build_edges_with_filter(pygraph& pg, const Eigen::Ref<DoubleMatrixR>& pts1,
                                 const std::size_t pts1_len,
                                 const Eigen::Ref<DoubleMatrixR>& pts2,
                                 const std::size_t pts2_len, const double epsilon,
                                 const Eigen::Ref<DoubleMatrixR> control_pts,
                                 const Eigen::Ref<BoolMatrixR> mask,
                                 const double percentage = 0.8);
}  // namespace ext
}  // namespace cliquematch
#endif /* ALIGNGRAPH_H */
