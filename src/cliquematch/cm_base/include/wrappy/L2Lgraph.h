#ifndef L2LGRAPH_H
#define L2LGRAPH_H

#include <cm_base/include/wrappy/ext_template.hpp>   // contains only templates
#include <cm_base/include/wrappy/wrap_template.hpp>  // contains only templates

// In L2LGraph.cpp

extern template struct relset<py::list, double>;

extern template std::vector<std::set<u32> >
edges_from_relsets<py::list, double, py::list, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<py::list, double>&,
    const double);

extern template std::vector<std::set<u32> >
efr_condition<py::list, double, py::list, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<py::list, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

extern template double dummy_comparison<py::list, double>(py::list&, u32, u32);

extern template struct GraphTemplate<py::list, double, py::list, double,
                                     double>;

#endif /* L2LGRAPH_H */

