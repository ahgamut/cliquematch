#include <ext/L2Lgraph.h>

template struct relset<py::list, double>;

template double dummy_comparison<py::list, double>(py::list&, std::size_t, std::size_t);

template struct GraphTemplate<py::list, py::list>;

void init_L2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<py::list, py::list>(mm, "L2LGraph");
}
