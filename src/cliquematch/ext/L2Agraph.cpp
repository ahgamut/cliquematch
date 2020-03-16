#include <ext/L2Agraph.h>

// required for relset to instantiate
template std::vector<std::set<std::size_t> >
edges_from_relsets<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<Eigen::Ref<DoubleMatrixR>, double>&, const double);

template std::vector<std::set<std::size_t> >
efr_condition<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
    std::size_t&, std::size_t&, const relset<py::list, double>&,
    const relset<Eigen::Ref<DoubleMatrixR>, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

// Specializing ctors to account for euclidean default
template <>
GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>::
    GraphTemplate(py::list& pts1, std::size_t pts1_len, Eigen::Ref<DoubleMatrixR>& pts2,
                  std::size_t pts2_len,
                  std::function<double(py::list&, std::size_t, std::size_t)> d1,
                  bool is_d1_symmetric)
    : GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
          pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric, euclidean, true)
{
}

template <>
GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double,
              double>::GraphTemplate(py::list& pts1, std::size_t pts1_len,
                                     Eigen::Ref<DoubleMatrixR>& pts2,
                                     std::size_t pts2_len)
    : GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
          pts1, pts1_len, pts2, pts2_len, dummy_comparison<py::list, double>, true,
          euclidean, true)
{
}

template struct GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double,
                              double>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_L2Agraph(pybind11::module& mm)
{
    init_GraphTemplate<py::list, double, Eigen::Ref<DoubleMatrixR>, double, double>(
        mm, "L2AGraph");
}
