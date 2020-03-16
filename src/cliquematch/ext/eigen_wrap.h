// just to define the relationship between Eigen and wrapping library
// and the main types to be used

#ifndef EIGEN_WRAP_H
#define EIGEN_WRAP_H

#pragma message("Using pybind11+Eigen, include Eigen directory location")
#include <Eigen/Dense>
// I want to have some vectorizing power from Eigen + Numpy defaults to Row-Major
// https://pybind11.readthedocs.io/en/stable/advanced/cast/eigen.html
using DoubleMatrixR =
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using BoolMatrixR =
    Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

#endif /* EIGEN_WRAP_H */

