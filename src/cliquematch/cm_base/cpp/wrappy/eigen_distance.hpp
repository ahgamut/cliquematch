#ifndef EIGEN_DISTANCE_H
#define EIGEN_DISTANCE_H

// Distance functions for 2D Numpy arrays/Eigen Matrices
#include <cm_base/include/wrappy/eigen_wrap.h>

template <typename DerivedMat>
double euc_dist0(const Eigen::MatrixBase<DerivedMat>& p,
                 const Eigen::MatrixBase<DerivedMat>& q) {
    return (p - q).norm();
}

double euclidean(Eigen::Ref<matrix>&, unsigned int, unsigned int);

#endif /* EIGEN_DISTANCE_H */
