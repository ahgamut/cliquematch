#ifndef ALIGNGRAPH_H
#define ALIGNGRAPH_H

#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    class AlignGraph : public A2AGraph
    {
       public:
        void build_edges_with_filter(Eigen::Ref<DoubleMatrixR>& pts1,
                                     std::size_t pts1_Len,
                                     Eigen::Ref<DoubleMatrixR>& pts2,
                                     std::size_t pts2_len,
                                     Eigen::Ref<DoubleMatrixR> control_pts,
                                     Eigen::Ref<BoolMatrixR> mask,
                                     double percentage = 0.8);
    };
}  // namespace ext
}  // namespace cliquematch
#endif /* ALIGNGRAPH_H */
