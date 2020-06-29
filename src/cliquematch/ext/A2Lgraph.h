#ifndef A2LGRAPH_H
#define A2LGRAPH_H

#include <ext/L2Lgraph.h>
#include <ext/A2Agraph.h>

// In A2LGraph.cpp

extern template class GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object, double>;

#endif /* A2LGRAPH_H */

