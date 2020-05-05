#include <ext/A2Agraph.h>

// required for relset to instantiate
template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

// required for GraphTemplate to instantiate
// specializing the template for the Eigen case as L2 Norm
template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len,
    Eigen::Ref<DoubleMatrixR>& pts2, std::size_t pts2_len,
    std::function<double(Eigen::Ref<DoubleMatrixR>&, std::size_t, std::size_t)> d1,
    bool is_d1_symmetric)
    : GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>(
          pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric, euclidean, true)
{
}
template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len,
    Eigen::Ref<DoubleMatrixR>& pts2, std::size_t pts2_len)
    : GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>(
          pts1, pts1_len, pts2, pts2_len, euclidean, true, euclidean, true)
{
}

template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Agraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>(
        mm, "A2AGraph");
}
