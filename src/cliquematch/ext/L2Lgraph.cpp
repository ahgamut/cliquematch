#include <ext/L2Lgraph.h>

namespace cliquematch
{
namespace ext
{
    template struct relset<py::object, double>;
    template double dummy_comparison(const py::object&, const u64, const u64);
}  // namespace ext

void init_L2Lgraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<pybind11::object, pybind11::object>(mm);
}
}  // namespace cliquematch
