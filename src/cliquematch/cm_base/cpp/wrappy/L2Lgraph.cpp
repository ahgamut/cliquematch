#include <cm_base/include/wrappy/gtpl.cpp>       // contains only templates
#include <cm_base/include/wrappy/wrap_gtpl.cpp>  // contains only templates

namespace py = pybind11;

template struct relset<py::list, double>;

template std::vector<std::set<u32> >
edges_from_relsets<py::list, double, py::list, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<py::list, double>&,
    const double);

template std::vector<std::set<u32> >
efr_condition<py::list, double, py::list, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<py::list, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

template double dummy_comparison<py::list, double>(py::list&, u32, u32);

template struct GraphTemplate<py::list, double, py::list, double, double>;

void init_L2Lgraph(pybind11::module& mm) {
    init_GraphTemplate<py::list, double, py::list, double, double>(mm,
                                                                   "L2LGraph");
}
