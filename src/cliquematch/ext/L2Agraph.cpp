#include <ext/A2Agraph.h>
#include <ext/L2Lgraph.h>

namespace cliquematch
{
void init_L2Agraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<pybind11::object, Eigen::Ref<DoubleMatrixR>>(mm);
}
}  // namespace cliquematch
