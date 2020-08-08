#include <ext/A2Lgraph.h>

namespace cliquematch
{
namespace ext
{
    template class GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object>;
}  // namespace ext
void init_A2Lgraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, pybind11::object>(mm,
                                                                         "A2LGraph");
}
}  // namespace cliquematch
