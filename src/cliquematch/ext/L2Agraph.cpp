#include <ext/L2Agraph.h>

namespace cliquematch
{
namespace ext
{
    template class GraphTemplate<py::object, Eigen::Ref<DoubleMatrixR>>;
}
void init_L2Agraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<pybind11::object, Eigen::Ref<DoubleMatrixR>>(mm,
                                                                         "L2AGraph");
}
}  // namespace cliquematch
