#ifndef POINTSET_H
#define POINTSET_H

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

#include <triples/triples.h>
#include <set>
#include <vector>

struct pointset {
    u32 N;
    double* pts;  // this is a horrible thing
    /* TODO: Remove raw pointer usage by using Eigen + higher language guards
     * <12-03-19, gnv3> */
    std::vector<dist_2d> dists;

    pointset();
    pointset(u32 N, double* pts);
    pointset(u32 N);
    pointset(u32 N, Eigen::Ref<matrix> pts);

    void fill_dists();
    void disp();
    Eigen::Map<matrix> get_mat() { return Eigen::Map<matrix>(this->pts, N, 2); }
};

void pts_reader(const char* filename, pointset&, pointset&, double&);

std::vector<std::set<u32> > edges_from_pointsets(u32& n_vert, u32& n_edges,
                                                 pointset&, pointset&,
                                                 const u32 choice = 1,
                                                 const double epsilon = 0.001);

std::vector<std::set<u32> > edges_filt_pointsets(u32& n_vert, u32& n_edges,
                                                 pointset&, pointset&,
                                                 Eigen::Ref<matrix> control,
                                                 Eigen::Ref<matmask> mask,
                                                 const double epsilon = 0.001,
                                                 const double percentage = 0.8);

#endif /* POINTSET_H */

