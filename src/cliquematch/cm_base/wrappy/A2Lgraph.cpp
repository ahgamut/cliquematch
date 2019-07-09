#include <cm_base/wrappy/eigrel.h>
#include <pybind11/eigen.h>
#include <cm_base/wrappy/gtpl/gtpl.cpp>       // contains only templates
#include <cm_base/wrappy/gtpl/wrap_gtpl.cpp>  // contains only templates

// required for relset to instantiate
extern template struct relset<Eigen::Ref<matrix>, double>;
extern template struct relset<py::list, double>;

template std::vector<std::set<u32> >
edges_from_relsets<Eigen::Ref<matrix>, double, py::list, double, double>(
    u32&, u32&, relset<Eigen::Ref<matrix>, double>&, relset<py::list, double>&,
    const double);

template std::vector<std::set<u32> >
efr_condition<Eigen::Ref<matrix>, double, py::list, double, double>(
    u32&, u32&, relset<Eigen::Ref<matrix>, double>&, relset<py::list, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

template <>
GraphTemplate<Eigen::Ref<matrix>, double, py::list, double,
              double>::GraphTemplate(Eigen::Ref<matrix>& pts1, u32 pts1_len,
                                     py::list& pts2, u32 pts2_len)
    : GraphTemplate<Eigen::Ref<matrix>, double, py::list, double, double>(
          pts1, pts1_len, pts2, pts2_len, eucdist, true,
          dummy_comparison<py::list, double>, true) {}

template struct GraphTemplate<Eigen::Ref<matrix>, double, py::list, double,
                              double>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_A2Lgraph(pybind11::module& mm) {
    init_GraphTemplate<Eigen::Ref<matrix>, double, py::list, double, double>(
        mm, "A2LGraph");
}

