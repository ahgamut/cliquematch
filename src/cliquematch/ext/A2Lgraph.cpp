#include <ext/A2Agraph.h>
#include <ext/L2Lgraph.h>

namespace cliquematch
{
void init_A2Lgraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, pybind11::object>(mm);
}
}  // namespace cliquematch
