#include <ext/eigen_distance.hpp>

double euclidean(Eigen::Ref<matrix>& ll, std::size_t i, std::size_t j)
{
    return euc_dist0(ll.row(i), ll.row(j));
}

