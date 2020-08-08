#include <ext/L2Lgraph.h>

namespace cliquematch
{
namespace ext
{
    template struct relset<py::object, double>;
    template double dummy_comparison<py::object, double>(py::object&, std::size_t,
                                                         std::size_t);
    template class GraphTemplate<py::object, py::object>;
}  // namespace ext

void init_L2Lgraph(pybind11::module& mm)
{
    ext::init_GraphTemplate<pybind11::object, pybind11::object>(mm, "L2LGraph");
}
}  // namespace cliquematch
