#ifndef L2AGRAPH_H
#define L2AGRAPH_H

#include <ext/L2Lgraph.h>
#include <ext/A2Agraph.h>

namespace cliquematch
{
namespace ext
{
    // In L2Agraph.cpp
    extern template class GraphTemplate<py::object, Eigen::Ref<DoubleMatrixR>>;
    using L2AGraph = GraphTemplate<py::object, Eigen::Ref<DoubleMatrixR>>;
}  // namespace ext
}  // namespace cliquematch

#endif /* L2AGRAPH_H */

