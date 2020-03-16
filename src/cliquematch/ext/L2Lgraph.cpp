#include <ext/L2Lgraph.h>

template struct relset<py::list, double>;

template std::vector<std::set<std::size_t> >
edges_from_relsets<py::list, double, py::list>(std::size_t&, std::size_t&,
                                               const relset<py::list, double>&,
                                               const relset<py::list, double>&,
                                               const double);

template std::vector<std::set<std::size_t> > efr_condition<py::list, double, py::list>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<py::list, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

template double dummy_comparison<py::list, double>(py::list&, std::size_t, std::size_t);

template struct GraphTemplate<py::list, double, py::list>;

void init_L2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<py::list, double, py::list>(mm, "L2LGraph");
}
