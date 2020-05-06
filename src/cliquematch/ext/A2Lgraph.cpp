#include <ext/A2Lgraph.h>

template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list>(mm, "A2LGraph");
}

