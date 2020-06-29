#include <ext/L2Lgraph.h>

template struct relset<py::object, double>;

template double dummy_comparison<py::object, double>(py::object&, std::size_t,
                                                     std::size_t);

template class GraphTemplate<py::object, py::object>;

void init_L2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<py::object, py::object>(mm, "L2LGraph");
}
