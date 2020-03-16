#include <ext/A2Agraph.h>

// required for relset to instantiate
template struct relset<Eigen::Ref<matrix>, double>;

template std::vector<std::set<std::size_t> >
edges_from_relsets<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<matrix>, double>&,
    const relset<Eigen::Ref<matrix>, double>&, const double);

template std::vector<std::set<std::size_t> >
efr_condition<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<matrix>, double>&,
    const relset<Eigen::Ref<matrix>, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

// required for GraphTemplate to instantiate
// specializing the template for the Eigen case as L2 Norm
template <>
GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>::
    GraphTemplate(
        Eigen::Ref<matrix>& pts1, std::size_t pts1_len, Eigen::Ref<matrix>& pts2,
        std::size_t pts2_len,
        std::function<double(Eigen::Ref<matrix>&, std::size_t, std::size_t)> d1,
        bool is_d1_symmetric)
    : GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
          pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric, euclidean, true)
{
}
template <>
GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double,
              double>::GraphTemplate(Eigen::Ref<matrix>& pts1, std::size_t pts1_len,
                                     Eigen::Ref<matrix>& pts2, std::size_t pts2_len)
    : GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
          pts1, pts1_len, pts2, pts2_len, euclidean, true, euclidean, true)
{
}

template struct GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double,
                              double>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Agraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
        mm, "A2AGraph");
}
