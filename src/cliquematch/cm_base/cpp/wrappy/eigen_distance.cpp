#include <cm_base/cpp/wrappy/eigen_distance.hpp>

double euclidean(Eigen::Ref<matrix>& ll, unsigned int i, unsigned int j) {
    return euc_dist0(ll.row(i), ll.row(j));
}

