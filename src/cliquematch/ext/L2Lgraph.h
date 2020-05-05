#ifndef L2LGRAPH_H
#define L2LGRAPH_H

#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In L2LGraph.cpp

extern template struct relset<py::list, double>;

extern template double dummy_comparison<py::list, double>(py::list&, std::size_t,
                                                          std::size_t);

extern template struct GraphTemplate<py::list, py::list>;

#endif /* L2LGRAPH_H */

