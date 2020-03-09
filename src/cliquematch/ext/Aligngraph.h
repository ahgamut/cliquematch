#ifndef ALIGNGRAPH_H
#define ALIGNGRAPH_H

#include <ext/A2Agraph.h>

using _A2Agraph = GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>,
				double, double>;

class Aligngraph : public _A2Agraph {
   public:
    Aligngraph(Eigen::Ref<matrix>& pts1, unsigned int pts1_len,
	       Eigen::Ref<matrix>& pts2, unsigned int pts2_len)
	: _A2Agraph(pts1, pts1_len, pts2, pts2_len, euclidean, true, euclidean,
		    false) {}
    void build_edges_with_filter(Eigen::Ref<matrix>& pts1,
				 Eigen::Ref<matrix>& pts2,
				 Eigen::Ref<matrix> control_pts,
				 Eigen::Ref<matmask> mask,
				 double percentage = 0.8);
};

#endif /* ALIGNGRAPH_H */
