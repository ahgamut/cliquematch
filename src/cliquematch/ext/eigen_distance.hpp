#ifndef EIGEN_DISTANCE_H
#define EIGEN_DISTANCE_H

// Distance functions for 2D Numpy arrays/Eigen Matrices
#include <ext/eigen_wrap.h>

template <typename DerivedMat>
double euc_dist0(const Eigen::MatrixBase<DerivedMat>& p,
                 const Eigen::MatrixBase<DerivedMat>& q)
{
    return (p - q).norm();
}

#endif /* EIGEN_DISTANCE_H */
