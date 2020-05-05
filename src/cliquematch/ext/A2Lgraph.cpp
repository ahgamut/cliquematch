#include <ext/A2Lgraph.h>

template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list>::GraphTemplate(
    Eigen::Ref<DoubleMatrixR>& pts1, std::size_t pts1_len, py::list& pts2,
    std::size_t pts2_len)
    : GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list, double>(
          pts1, pts1_len, pts2, pts2_len, euclidean, true,
          dummy_comparison<py::list, double>, true)
{
}

template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, py::list>(mm, "A2LGraph");
}

