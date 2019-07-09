// just to define the relationship between Eigen and wrapping library

#ifndef EIGREL_H
#define EIGREL_H

#if WRAPPY == 1
#pragma message("Using pybind11+Eigen")
#include <Eigen/Dense>
using matrix =
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using matmask =
    Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
// I want to have some vectorizing power from Eigen + Numpy has only Row-Major
// https://pybind11.readthedocs.io/en/stable/advanced/cast/eigen.html

#elif WRAPR == 1
#pragma message("Using RcppEigen")
#include <RcppEigen.h>
using matrix = Eigen::MatrixXd;
using matmask = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>;

#else
#pragma message("Error: no pybind11 or Rcpp")
#endif

template <typename DerivedMat>
double eucdist0(const Eigen::MatrixBase<DerivedMat>& p,
                const Eigen::MatrixBase<DerivedMat>& q) {
    // auto t = (p - q).array();
    // double answer = t.pow(2).sum().sqrt();
    return (p - q).norm();
}

double eucdist(Eigen::Ref<matrix>&, unsigned int, unsigned int);

#endif /* EIGREL_H */

