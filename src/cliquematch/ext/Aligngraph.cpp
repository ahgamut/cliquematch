#include <ext/Aligngraph.h>

using ULongMatrixR =
    Eigen::Matrix<std::size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

double inline filter_score(Eigen::Ref<DoubleMatrixR> control,
                           Eigen::Ref<BoolMatrixR> msk, Eigen::RowVector2d& c,
                           Eigen::Ref<DoubleMatrixR> rot_con,
                           Eigen::Ref<ULongMatrixR> RCU,
                           const Eigen::Ref<DoubleMatrixR>& M1, std::size_t i1,
                           std::size_t j1, const Eigen::Ref<DoubleMatrixR>& M2,
                           std::size_t i2, std::size_t j2)
{
    static Eigen::Matrix<double, 4, 4> coeffs;
    static Eigen::Matrix<double, 4, 1> rhs;
    static Eigen::Matrix<double, 4, 1> answer;

    std::size_t k, m_r = msk.rows(), m_c = msk.cols();
    double a, b, msk_score = 0;

    static double x1, y1, x2, y2, x3, y3, x4, y4;
    x1 = M2(i2, 0), y1 = M2(i2, 1);
    x2 = M2(j2, 0), y2 = M2(j2, 1);
    x3 = M1(i1, 0), y3 = M1(i1, 1);
    x4 = M1(j1, 0), y4 = M1(j1, 1);
    if (x1 - x2 == 0 && y1 - y2 == 0) return msk_score;

    coeffs << x1, -y1, 1, 0, y1, x1, 0, 1, x2, -y2, 1, 0, y2, x2, 0, 1;
    rhs << x3, y3, x4, y4;
    answer = coeffs.partialPivLu().solve(rhs);
    a = answer(0, 0);
    b = answer(1, 0);
    c(0) = answer(2, 0);
    c(1) = answer(3, 0);

    rot_con.col(0) = a * control.col(0) - b * control.col(1);
    rot_con.col(1) = b * control.col(0) + a * control.col(1);
    rot_con.rowwise() += c;

    RCU = rot_con.cast<std::size_t>();
    for (k = 0; k < static_cast<std::size_t>(rot_con.rows()); k++)
    {
        if (RCU(k, 0) < m_r && RCU(k, 1) < m_c && msk(RCU(k, 0), RCU(k, 1)))
            msk_score++;
    }
    msk_score /= rot_con.rows();
    return msk_score;
}

void Aligngraph::build_edges_with_filter(Eigen::Ref<DoubleMatrixR>& pts1,
                                         Eigen::Ref<DoubleMatrixR>& pts2,
                                         Eigen::Ref<DoubleMatrixR> control_pts,
                                         Eigen::Ref<BoolMatrixR> mask,
                                         double percentage)
{
    Eigen::RowVector2d _c;
    DoubleMatrixR _rot_con(control_pts.rows(), control_pts.cols());
    ULongMatrixR _RCU(control_pts.rows(), control_pts.cols());
    auto rule_func = [&control_pts, &mask, &_c, &_rot_con, &_RCU, &percentage](
                         Eigen::Ref<DoubleMatrixR>& p1, std::size_t i1, std::size_t j1,
                         Eigen::Ref<DoubleMatrixR>& p2, std::size_t i2,
                         std::size_t j2) -> bool {
        return filter_score(control_pts, mask, _c, _rot_con, _RCU, p1, i1, j1, p2, i2,
                            j2) >= percentage;
    };
    this->build_edges_with_condition(pts1, pts2, rule_func, false);
}

void init_Aligngraph(pybind11::module& mm)
{
    using namespace pybind11;
    using a2a = GraphTemplate<Eigen::Ref<DoubleMatrixR>, double,
                              Eigen::Ref<DoubleMatrixR>, double, double>;
    class_<Aligngraph, a2a>(mm, "AlignGraph")
        .def(init<Eigen::Ref<DoubleMatrixR>&, std::size_t, Eigen::Ref<DoubleMatrixR>&,
                  std::size_t>())
        .def("build_edges_with_filter", &Aligngraph::build_edges_with_filter);
}