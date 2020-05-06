#ifndef A2LGRAPH_H
#define A2LGRAPH_H

#include <ext/L2Lgraph.h>
#include <ext/A2Agraph.h>

// In A2LGraph.cpp

extern template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list, double>;

#endif /* A2LGRAPH_H */

