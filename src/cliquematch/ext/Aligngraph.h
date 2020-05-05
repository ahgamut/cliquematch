#ifndef ALIGNGRAPH_H
#define ALIGNGRAPH_H

#include <ext/A2Agraph.h>

using _A2Agraph = GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;

class Aligngraph : public _A2Agraph
{
   public:
    Aligngraph(Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len,
               Eigen::Ref<DoubleMatrixR>& pts2, std::size_t pts2_len)
        : _A2Agraph(pts1, pts1_len, pts2, pts2_len, euclidean, true, euclidean, true)
    {
    }
    void build_edges_with_filter(Eigen::Ref<DoubleMatrixR>& pts1,
                                 Eigen::Ref<DoubleMatrixR>& pts2,
                                 Eigen::Ref<DoubleMatrixR> control_pts,
                                 Eigen::Ref<BoolMatrixR> mask, double percentage = 0.8);
};

#endif /* ALIGNGRAPH_H */
