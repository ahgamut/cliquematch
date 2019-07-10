#include <cm_base/pointset/pointset.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

double inline get_a(double dx1, double dy1, double dx2, double dy2) {
    return (dx2 * dx1 + dy2 * dy1) / (dx1 * dx1 + dy1 * dy1);
}

double inline get_b(double dx1, double dy1, double dx2, double dy2) {
    return (dy2 * dx1 - dy1 * dx2) / (dx1 * dx1 + dy1 * dy1);
}

double inline steve_rule(Eigen::Map<matrix>& M1, Eigen::Map<matrix>& M2,
                         dist_2d& i, dist_2d& j, Eigen::Ref<matrix>& control,
                         Eigen::Ref<matmask>& msk, bool toggle = true) {
    // variables for Steve's condition
    static double x1, y1, x2, y2, x3, y3, x4, y4;
    static double a, b;
    static Eigen::RowVector2d c;
    static matrix rot_con(control.rows(), control.cols());
    static Eigen::Matrix<u32, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
        RCU(control.rows(), control.cols());
    static u32 k;
    static u32 m_r = msk.rows(), m_c = msk.cols();

    double msk_score = 0;

    x1 = M1(i.first, 0), y1 = M1(i.first, 1);
    x2 = M1(i.second, 0), y2 = M1(i.second, 1);

    if (!toggle) {
        x3 = M2(j.first, 0), y3 = M2(j.first, 1);
        x4 = M2(j.second, 0), y4 = M2(j.second, 1);
    } else {
        x4 = M2(j.first, 0), y4 = M2(j.first, 1);
        x3 = M2(j.second, 0), y3 = M2(j.second, 1);
    }
    if (x1 - x2 == 0 && y1 - y2 == 0) return 0;
    a = get_a(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    b = get_b(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    c(0) = x3 + b * y1 - a * x1;
    c(1) = y3 - b * x1 - a * y1;

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

vector<set<u32> > edges_filt_pointsets(u32& n_vert, u32& n_edges, pointset& s1,
                                       pointset& s2,
                                       Eigen::Ref<matrix> control_pts,
                                       Eigen::Ref<matmask> mask,
                                       const double epsilon,
                                       const double percentage) {
    u32 M = s1.N, N = s2.N, i, j;

    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0) {
        throw std::runtime_error(
            "One of the point sets is empty (psgraph initialization error)\n");
    }

    vector<set<u32> > Edges(n_vert + 1);

    u32 v1, v2;

    dist_2d* base = s2.dists.data();
    u32 len = N * (N - 1) / 2;

    double cur_ub = 0, cur_lb = 0;
    u32 ub_loc = len, lb_loc = 0;

    short found1, found2;
    auto mat1 = s1.get_mat();
    auto mat2 = s2.get_mat();
    u32 steve_caught = 0;

    for (i = 0; i < M * (M - 1) / 2; i++) {
        cur_lb = s1.dists[i].dist - epsilon;
        cur_ub = cur_lb + 2 * epsilon;
        found1 = binary_find2(base, len, cur_lb, lb_loc);
        if (found1 == -1) break;
        found2 = binary_find2(base, len, cur_ub, ub_loc);
        if (found2 == -1) ub_loc = len - 1;

        if (lb_loc > ub_loc) {
            cerr << "Overflow glitch?!?!\n";
            break;
        }

        // cerr<<found1<<" "<<lb_loc<<","<<found2<<" "<<ub_loc<<": "<<cur_lb<<"
        // < "<<(*(s2.dists))[lb_loc].dist<<" ; "<<cur_ub<<" >
        // "<<(*(s2.dists))[ub_loc].dist<<"\n";
        //

        // Steve's Condition in play now

        for (j = lb_loc; j <= ub_loc; j++) {
            // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
            v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

            if (steve_rule(mat2, mat1, s2.dists[j], s1.dists[i], control_pts,
                           mask, false) > percentage) {
                Edges[v1].insert(v2);
                Edges[v2].insert(v1);
                n_edges++;
            } else
                steve_caught++;

            // if d(i,j) approx= d(i',j') then edge ALSO between (i,j') and
            // (j,i')
            v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

            if (steve_rule(mat2, mat1, s2.dists[j], s1.dists[i], control_pts,
                           mask, true) > percentage) {
                Edges[v1].insert(v2);
                Edges[v2].insert(v1);
                n_edges++;
            } else
                steve_caught++;
        }
    }

    cout << "steve's rule eliminated:  " << steve_caught << " edges \n";
    return Edges;
}
