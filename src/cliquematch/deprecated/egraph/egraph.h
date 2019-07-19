#ifndef EGRAPH_H
#define EGRAPH_H

#include <relset/eigrel.h>
#include <wrappy/graph2/graph2.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>

template <typename dtype>
using ndarray = pybind11::array_t<dtype, pybind11::array::c_style |
                                             pybind11::array::forcecast>;

struct Egraph : public graph2 {
    pset ps1, ps2;
    double pts_epsilon;

    Egraph();
    Egraph(Eigen::Ref<matrix> pts1, u32 pts1_len, Eigen::Ref<matrix> pts2,
           u32 pts2_len,
           std::function<double(Eigen::Ref<matrix>&, u32, u32)> d1,
           std::function<double(Eigen::Ref<matrix>&, u32, u32)> d2,
           bool is_d1_symmetric = true, bool is_d2_symmetric = true);

    Egraph(Eigen::Ref<matrix> pts1, u32 pts1_len, Eigen::Ref<matrix> pts2,
           u32 pts2_len,
           std::function<double(Eigen::Ref<matrix>&, u32, u32)> d1,
           bool is_d1_symmetric = true)
        : Egraph(pts1, pts1_len, pts2, pts2_len, d1, eucdist, is_d1_symmetric,
                 true) {}

    Egraph(Eigen::Ref<matrix> pts1, u32 pts1_len, Eigen::Ref<matrix> pts2,
           u32 pts2_len)
        : Egraph(pts1, pts1_len, pts2, pts2_len, eucdist, eucdist, true, true) {
    }

    bool build_edges();
    bool build_edges_with_condition(
        Eigen::Ref<matrix>& pts1, Eigen::Ref<matrix>& pts2,
        std::function<bool(Eigen::Ref<matrix>&, u32, u32, Eigen::Ref<matrix>&,
                           u32, u32)>
            cfunc,
        bool use_cfunc_only = false);
    std::string showdata(bool fully = false);
    pybind11::list get_correspondence();
    /*    pybind11::list get_correspondence(bool return_indices = false,
                                          ndarray<u32> clique);
                                          */
};

#endif /* EGRAPH_H */
