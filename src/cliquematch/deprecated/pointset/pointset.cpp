#include <cm_base/pointset/pointset.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>

#define euc_dist(px, py, qx, qy) \
    sqrt(((px) - (qx)) * ((px) - (qx)) + ((py) - (qy)) * ((py) - (qy)))

using namespace std;

pointset::pointset() {
    this->N = 0;
    this->pts = nullptr;
}

pointset::pointset(u32 N, Eigen::Ref<matrix> mat) {
    this->N = N;
    this->pts = mat.data();
    if (this->dists.size() != 0) vector<dist_2d>().swap(this->dists);
    this->dists = vector<dist_2d>(N * (N - 1) / 2);
}

pointset::pointset(u32 N, double* pts) {
    this->N = N;
    this->pts = pts;
    if (this->dists.size() != 0) vector<dist_2d>().swap(this->dists);
    this->dists = vector<dist_2d>(N * (N - 1) / 2);
}

pointset::pointset(u32 N) {
    this->N = N;
    this->pts = new double[2 * N];
    if (this->dists.size() != 0) vector<dist_2d>().swap(this->dists);
    this->dists = vector<dist_2d>(N * (N - 1) / 2);
}

void pointset::fill_dists() {
    u32 i, j, count = 0;
    auto mat = this->get_mat();
    for (i = 0; i < this->N; i++) {
        for (j = i + 1; j < this->N; j++) {
            dist_2d& temp = this->dists[count];
            temp.first = i;
            temp.second = j;
            temp.dist =
                euc_dist((mat(i, 0)), (mat(i, 1)), (mat(j, 0)), (mat(j, 1)));
            count++;
        }
    }
    sort(this->dists.begin(), this->dists.end());
}

void pointset::disp() {
    for (u32 i = 0; i < (N * (N - 1)) / 2; i++) this->dists[i].disp();
}

void eps_bounds(double& LB, double& UB, const double val, const double& eps,
                const u32 ch = 1) {
    if (ch == 1) {
        // tolerance is constant
        LB = val - eps;
        UB = LB + 2 * eps;
    }

    else if (ch == 2) {
        // tolerance is dependent on the distance
        LB = eps > 0.02 * val ? val * 0.98 : val - eps;
        UB = eps > 0.02 * val ? val * 1.02 : val + eps;
    }

    else {
        // compromise
        LB = eps / 2 > 0.02 * val ? eps / 2 : 0.02 * val;
        LB = eps < LB ? eps : LB;
        UB = val + LB;
        LB = val - LB;
    }
}

void pts_reader(const char* filename, pointset& s1, pointset& s2,
                double& epsilon) {
    ifstream f(filename, ios::in);
    u32 M, N, i;
    epsilon = 0.001;
    double x, y;

    if (!f.is_open()) {
        throw std::runtime_error(
            "Unable to open file! Pointsets (epsilon-M-pts-N-pts)\n");
        return;
    }
    f >> epsilon;

    f >> M;
    s1.N = M;
    s1.pts = new double[M];
    auto s1_pts = s1.get_mat();
    for (i = 0; i < M; i++) {
        f >> x >> y;
        s1_pts(i, 0) = x;
        s1_pts(i, 1) = y;
    }
    s1.fill_dists();
    s1.disp();
    cerr << "\n\n";

    f >> N;
    s2.N = N;
    s2.pts = new double[N];
    auto s2_pts = s2.get_mat();
    for (i = 0; i < N; i++) {
        f >> x >> y;
        s2_pts(i, 0) = x;
        s2_pts(i, 1) = y;
    }
    s2.fill_dists();
    s2.disp();

    f.close();
}

vector<set<u32> > edges_from_pointsets(u32& n_vert, u32& n_edges, pointset& s1,
                                       pointset& s2, const u32 choice,
                                       const double epsilon) {
    u32 M = s1.N, N = s2.N, i, j;

    // double x,y;
    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0) {
        throw std::runtime_error(
            "One of the point sets is empty (psgraph initialization error)\n");
        vector<set<u32> > Edges;
        return Edges;
    }

    vector<set<u32> > Edges(n_vert + 1);
    // cerr<<"S1 has "<<M<<" marked points, S2 has "<<N<<" marked points\n";

    u32 v1, v2;

    dist_2d* base = s2.dists.data();
    u32 len = N * (N - 1) / 2;

    double cur_ub = 0, cur_lb = 0;
    u32 ub_loc = len, lb_loc = 0;

    short found1, found2;

    for (i = 0; i < M * (M - 1) / 2; i++) {
        eps_bounds(cur_lb, cur_ub, s1.dists[i].dist, epsilon, choice);

        // cur_lb = (*(s1.dists))[i].dist - epsilon;
        // cur_ub = cur_lb + 2*epsilon;

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

        for (j = lb_loc; j <= ub_loc; j++) {
            // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
            v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

            Edges[v1].insert(v2);
            Edges[v2].insert(v1);
            n_edges++;

            // if d(i,j) approx= d(i',j') then edge ALSO between (i,j') and
            // (j,i')
            v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

            Edges[v1].insert(v2);
            Edges[v2].insert(v1);
            n_edges++;
        }
    }

    return Edges;
}
