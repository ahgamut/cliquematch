#include <cm_base/wrappy/eigrel.h>
#include <pybind11/eigen.h>
#include <cm_base/wrappy/gtpl/gtpl.cpp>       // contains only templates
#include <cm_base/wrappy/gtpl/wrap_gtpl.cpp>  // contains only templates

extern template struct relset<Eigen::Ref<matrix>, double>;
extern template struct relset<py::list, double>;

// required for relset to instantiate
template std::vector<std::set<u32> >
edges_from_relsets<py::list, double, Eigen::Ref<matrix>, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<Eigen::Ref<matrix>, double>&,
    const double);

template std::vector<std::set<u32> >
efr_condition<py::list, double, Eigen::Ref<matrix>, double, double>(
    u32&, u32&, relset<py::list, double>&, relset<Eigen::Ref<matrix>, double>&,
    const double, std::function<bool(u32, u32, u32, u32)>, bool);

// Specializing ctors to account for eucdist default
template <>
GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>::
    GraphTemplate(py::list& pts1, u32 pts1_len, Eigen::Ref<matrix>& pts2,
                  u32 pts2_len, std::function<double(py::list&, u32, u32)> d1,
                  bool is_d1_symmetric)
    : GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>(
          pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric, eucdist, true) {}

template <>
GraphTemplate<py::list, double, Eigen::Ref<matrix>, double,
              double>::GraphTemplate(py::list& pts1, u32 pts1_len,
                                     Eigen::Ref<matrix>& pts2, u32 pts2_len)
    : GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>(
          pts1, pts1_len, pts2, pts2_len, dummy_comparison<py::list, double>,
          true, eucdist, true) {}

template struct GraphTemplate<py::list, double, Eigen::Ref<matrix>, double,
                              double>;

// required for wrapper template to instantiate
// for calling from cm_base.cpp
void init_L2Agraph(pybind11::module& mm) {
    init_GraphTemplate<py::list, double, Eigen::Ref<matrix>, double, double>(
        mm, "L2AGraph");
}
