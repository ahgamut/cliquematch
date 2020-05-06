#ifndef L2AGRAPH_H
#define L2AGRAPH_H

#include <ext/L2Lgraph.h>
#include <ext/A2Agraph.h>

// In L2Agraph.cpp

extern template struct GraphTemplate<py::list, Eigen::Ref<DoubleMatrixR>>;

#endif /* L2AGRAPH_H */

