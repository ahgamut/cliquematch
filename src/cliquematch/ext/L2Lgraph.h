#ifndef L2LGRAPH_H
#define L2LGRAPH_H

#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In L2LGraph.cpp

extern template struct relset<py::list, double>;

extern template std::vector<std::set<std::size_t> >
edges_from_relsets<py::list, double, py::list, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<py::list, double>&, const double);

extern template std::vector<std::set<std::size_t> >
efr_condition<py::list, double, py::list, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<py::list, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

extern template double dummy_comparison<py::list, double>(py::list&, std::size_t,
                                                          std::size_t);

extern template struct GraphTemplate<py::list, double, py::list, double, double>;

#endif /* L2LGRAPH_H */

