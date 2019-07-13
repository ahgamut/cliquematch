#ifndef EXT_SUB_H
#define EXT_SUB_H

#include <cm_base/include/wrappy/A2Agraph.h>
#include <cm_base/include/wrappy/A2Lgraph.h>
#include <cm_base/include/wrappy/L2Agraph.h>
#include <cm_base/include/wrappy/L2Lgraph.h>

namespace py = pybind11;

// For subclasses that need to use these base classes
using A2AGraph = struct GraphTemplate<Eigen::Ref<matrix>, double,
                                      Eigen::Ref<matrix>, double, double>;
using L2LGraph =
    struct GraphTemplate<py::list, double, py::list, double, double>;
using L2AGraph =
    struct GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>;
using A2LGraph =
    struct GraphTemplate<Eigen::Ref<matrix>, double, py::list, double, double>;

// The subclasses need to know that the template instantiation has been done for
// them externally

#endif /* EXT_SUB_H */

