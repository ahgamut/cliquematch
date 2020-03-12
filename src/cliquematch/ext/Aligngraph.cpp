#include <ext/Aligngraph.h>

using uint_matrix = Eigen::Matrix<unsigned int, Eigen::Dynamic, Eigen::Dynamic,
				  Eigen::RowMajor>;

using bool_matrix =
    Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

double inline filter_score(Eigen::Ref<matrix> control,
			   Eigen::Ref<bool_matrix> msk, Eigen::RowVector2d& c,
			   Eigen::Ref<matrix> rot_con,
			   Eigen::Ref<uint_matrix> RCU, Eigen::Ref<matrix>& M1,
			   unsigned int i1, unsigned int j1,
			   Eigen::Ref<matrix>& M2, unsigned int i2,
			   unsigned int j2) {
    static Eigen::Matrix<double, 4, 4> coeffs;
    static Eigen::Matrix<double, 4, 1> rhs;
    static Eigen::Matrix<double, 4, 1> answer;

    unsigned int k, m_r = msk.rows(), m_c = msk.cols();
    double a, b, msk_score = 0;

    static double x1, y1, x2, y2, x3, y3, x4, y4;
    x1 = M1(i1, 0), y1 = M1(i1, 1);
    x2 = M1(j1, 0), y2 = M1(j1, 1);
    x3 = M2(i2, 0), y3 = M2(i2, 1);
    x4 = M2(j2, 0), y4 = M2(j2, 1);
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

    RCU = rot_con.cast<u32>();
    for (k = 0; k < rot_con.rows(); k++) {
	if (RCU(k, 0) < m_r && RCU(k, 1) < m_c && msk(RCU(k, 0), RCU(k, 1)))
	    msk_score++;
    }
    msk_score /= rot_con.rows();
    return msk_score;
}

void Aligngraph::build_edges_with_filter(Eigen::Ref<matrix>& pts1,
					 Eigen::Ref<matrix>& pts2,
					 Eigen::Ref<matrix> control_pts,
					 Eigen::Ref<bool_matrix> mask,
					 double percentage) {
    Eigen::RowVector2d _c;
    matrix _rot_con(control_pts.rows(), control_pts.cols());
    uint_matrix _RCU(control_pts.rows(), control_pts.cols());
    auto rule_func = [&control_pts, &mask, &_c, &_rot_con, &_RCU, &percentage](
			 Eigen::Ref<matrix>& p1, unsigned int i1,
			 unsigned int j1, Eigen::Ref<matrix>& p2,
			 unsigned int i2, unsigned int j2) -> bool {
	return filter_score(control_pts, mask, _c, _rot_con, _RCU, p1, i1, j1,
			    p2, i2, j2) >= percentage;
    };
    this->build_edges_with_condition(pts1, pts2, rule_func, false);
}

void init_Aligngraph(pybind11::module& mm) {
    using namespace pybind11;
    using a2a = GraphTemplate<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>,
			      double, double>;
    class_<Aligngraph, a2a>(mm, "AlignGraph")
	.def(init<Eigen::Ref<matrix>&, unsigned int, Eigen::Ref<matrix>&,
		  unsigned int>())
	.def("build_edges_with_filter", &Aligngraph::build_edges_with_filter);
}
