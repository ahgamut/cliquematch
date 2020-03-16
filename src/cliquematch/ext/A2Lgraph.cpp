#include <ext/A2Lgraph.h>

template std::vector<std::set<std::size_t> >
edges_from_relsets<Eigen::Ref<DoubleMatrixR>, double, py::list, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<DoubleMatrixR>, double>&,
    const relset<py::list, double>&, const double);

template std::vector<std::set<std::size_t> >
efr_condition<Eigen::Ref<DoubleMatrixR>, double, py::list, double, double>(
    std::size_t&, std::size_t&, const relset<Eigen::Ref<DoubleMatrixR>, double>&,
    const relset<py::list, double>&, const double,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)>, bool);

template <>
GraphTemplate<Eigen::Ref<DoubleMatrixR>, double, py::list, double,
              double>::GraphTemplate(Eigen::Ref<DoubleMatrixR>& pts1,
                                     std::size_t pts1_len, py::list& pts2,
                                     std::size_t pts2_len)
    : GraphTemplate<Eigen::Ref<DoubleMatrixR>, double, py::list, double, double>(
          pts1, pts1_len, pts2, pts2_len, euclidean, true,
          dummy_comparison<py::list, double>, true)
{
}

template struct GraphTemplate<Eigen::Ref<DoubleMatrixR>, double, py::list, double,
                              double>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Lgraph(pybind11::module& mm)
{
    init_GraphTemplate<Eigen::Ref<DoubleMatrixR>, double, py::list, double, double>(
        mm, "A2LGraph");
}

