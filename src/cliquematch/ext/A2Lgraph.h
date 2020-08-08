#ifndef A2LGRAPH_H
#define A2LGRAPH_H

#include <ext/L2Lgraph.h>
#include <ext/A2Agraph.h>

// In A2LGraph.cpp
namespace cliquematch
{
namespace ext
{
    extern template class GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object, double>;
    using A2LGraph = GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object, double>;
}  // namespace ext
}  // namespace cliquematch
#endif /* A2LGRAPH_H */

