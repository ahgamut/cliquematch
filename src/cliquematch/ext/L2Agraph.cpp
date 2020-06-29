#include <ext/L2Agraph.h>

template class GraphTemplate<py::object, Eigen::Ref<DoubleMatrixR>>;

// required for wrapper template to instantiate
// for calling from core.cpp
void init_L2Agraph(pybind11::module& mm)
{
    init_GraphTemplate<py::object, Eigen::Ref<DoubleMatrixR>>(mm, "L2AGraph");
}
