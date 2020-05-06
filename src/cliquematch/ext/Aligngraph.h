#ifndef ALIGNGRAPH_H
#define ALIGNGRAPH_H

#include <ext/A2Agraph.h>

using _A2Agraph = GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;

class Aligngraph : public _A2Agraph
{
   public:
    void build_edges_with_filter(Eigen::Ref<DoubleMatrixR>& pts1, size_t pts1_Len,
                                 Eigen::Ref<DoubleMatrixR>& pts2, size_t pts2_len,
                                 Eigen::Ref<DoubleMatrixR> control_pts,
                                 Eigen::Ref<BoolMatrixR> mask, double percentage = 0.8);
};

#endif /* ALIGNGRAPH_H */
