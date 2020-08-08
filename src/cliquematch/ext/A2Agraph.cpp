#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    template <>
    double dummy_comparison<Eigen::Ref<DoubleMatrixR>, double>(
        Eigen::Ref<DoubleMatrixR>& ll, std::size_t i, std::size_t j)
    {
        return euc_dist0(ll.row(i), ll.row(j));
    }
    template struct relset<Eigen::Ref<DoubleMatrixR>, double>;
    template class GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>;
}  // namespace ext

// calling from core.cpp
void init_A2Agraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>>(
        mm, "A2AGraph");
}
}  // namespace cliquematch
