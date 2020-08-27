#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    template <>
    double dummy_comparison<Eigen::Ref<DoubleMatrixR>, double>(
        const Eigen::Ref<DoubleMatrixR>& ll, const std::size_t i, const std::size_t j)
    {
        return euc_dist0(ll.row(i), ll.row(j));
    }
    template bool build_edges(
        pygraph& pg, const Eigen::Ref<DoubleMatrixR>& pts1, const std::size_t pts1_len,
        const Eigen::Ref<DoubleMatrixR>& pts2, const std::size_t pts2_len,
        const double epsilon,
        const std::function<bool(const Eigen::Ref<DoubleMatrixR>&, const std::size_t,
                                 const std::size_t, const Eigen::Ref<DoubleMatrixR>&,
                                 const std::size_t, const std::size_t)>
            cfunc,
        std::function<double(const Eigen::Ref<DoubleMatrixR>&, const std::size_t,
                             const std::size_t)>
            d1,
        const bool is_d1_symmetric,
        std::function<double(const Eigen::Ref<DoubleMatrixR>&, const std::size_t,
                             const std::size_t)>
            d2,
        const bool is_d2_symmetric);
    template struct relset<Eigen::Ref<DoubleMatrixR>, double>;
}  // namespace ext

void init_A2Agraph(pybind11::module& m)
{
    ext::init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, Eigen::Ref<DoubleMatrixR>,
                            double>(m);
}

}  // namespace cliquematch
