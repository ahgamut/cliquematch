#include <ext/A2Lgraph.h>

template class GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object>;

// required for wrapper template to instantiate
// for calling from core.cpp
void init_A2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::object>(mm, "A2LGraph");
}

