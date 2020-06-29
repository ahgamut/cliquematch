#ifndef L2LGRAPH_H
#define L2LGRAPH_H

#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In L2LGraph.cpp

extern template struct relset<py::object, double>;

extern template double dummy_comparison<py::object, double>(py::object&, std::size_t,
                                                            std::size_t);

extern template class GraphTemplate<py::object, py::object>;

#endif /* L2LGRAPH_H */

