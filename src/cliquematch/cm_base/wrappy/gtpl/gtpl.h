#ifndef GTPL_H
#define GTPL_H

#include <cm_base/wrappy/graph2/graph2.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <cm_base/relset/relset.cpp>  //this contains only templates

template <typename List, typename Delta>
Delta dummy_comparison(List& ll, u32 i, u32 j) {
    throw std::runtime_error(
        "Can't have a distance function for an unknown type " +
        std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
    return (Delta)(0);
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
struct GraphTemplate : public graph2 {
    relset<List1, Delta1> ps1;
    relset<List2, Delta2> ps2;
    EpsType pts_epsilon;

    GraphTemplate();
    GraphTemplate(List1& pts1, u32 pts1_len, List2& pts2, u32 pts2_len,
                  std::function<Delta1(List1&, u32, u32)> d1,
                  bool is_d1_symmetric,
                  std::function<Delta2(List2&, u32, u32)> d2,
                  bool is_d2_symmetric);

    GraphTemplate(List1& pts1, u32 pts1_len, List2& pts2, u32 pts2_len,
                  std::function<Delta1(List1&, u32, u32)> d1,
                  bool is_d1_symmetric = true)
        : GraphTemplate(pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric,
                        dummy_comparison<List2, Delta2>, true) {}

    GraphTemplate(List1& pts1, u32 pts1_len, List2& pts2, u32 pts2_len)
        : GraphTemplate(pts1, pts1_len, pts2, pts2_len,
                        dummy_comparison<List1, Delta1>, true,
                        dummy_comparison<List2, Delta2>, true) {}

    bool build_edges(List1& pts1, List2& pts2);
    bool build_edges_with_condition(
        List1& pts1, List2& pts2,
        std::function<bool(List1&, u32, u32, List2&, u32, u32)> cfunc,
        bool use_cfunc_only = false);

    std::string showdata();
    pybind11::list get_correspondence();
    pybind11::list get_correspondence2(std::vector<u32>& clique);
};

#endif /* GTPL_H */

