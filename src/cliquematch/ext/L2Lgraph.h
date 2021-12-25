#ifndef L2LGRAPH_H
#define L2LGRAPH_H

#include <templates/ext_template.hpp>   // contains only templates
#include <templates/wrap_template.hpp>  // contains only templates

// In L2LGraph.cpp
namespace cliquematch
{
namespace ext
{
    extern template struct relset<py::object, double>;
    extern template double dummy_comparison<py::object, double>(const py::object&,
                                                                const u64, const u64);
}  // namespace ext
}  // namespace cliquematch

#endif /* L2LGRAPH_H */
