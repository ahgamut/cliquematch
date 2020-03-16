// just to define the relationship between Eigen and wrapping library
// and the main types to be used

#ifndef EIGEN_WRAP_H
#define EIGEN_WRAP_H

#if WRAPPY == 1
#pragma message("Using pybind11+Eigen, include Eigen directory location")
#include <Eigen/Dense>
using matrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using matmask = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
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

#endif /* EIGEN_WRAP_H */

