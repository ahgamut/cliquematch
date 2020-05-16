#include <ext/A2Agraph.h>

template <>
double dummy_comparison<Eigen::Ref<DoubleMatrixR>, double>(
    Eigen::Ref<DoubleMatrixR>& ll, std::size_t i, std::size_t j)
{
    return euc_dist0(ll.row(i), ll.row(j));
}

// required for relset to instantiate
template struct relset<Eigen::Ref<DoubleMatrixR>, double>;

template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;

// required for wrapper template to instantiate
// for calling from core.cpp
void init_A2Agraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>(
        mm, "A2AGraph");
}
