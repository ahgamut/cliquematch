#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    template <>
    double dummy_comparison<Eigen::Ref<DoubleMatrixR>, double>(
        const Eigen::Ref<DoubleMatrixR>& ll, const u64 i, const u64 j)
    {
        return euc_dist0(ll.row(i), ll.row(j));
    }
    template bool build_edges(
        pygraph& pg, const Eigen::Ref<DoubleMatrixR>& pts1, const u64 pts1_len,
        const Eigen::Ref<DoubleMatrixR>& pts2, const u64 pts2_len, const double epsilon,
        const std::function<bool(const Eigen::Ref<DoubleMatrixR>&, const u64, const u64,
                                 const Eigen::Ref<DoubleMatrixR>&, const u64,
                                 const u64)>
            cfunc,
        std::function<double(const Eigen::Ref<DoubleMatrixR>&, const u64, const u64)>
            d1,
        const bool is_d1_symmetric,
        std::function<double(const Eigen::Ref<DoubleMatrixR>&, const u64, const u64)>
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
