#ifndef RELSET_HPP
#define RELSET_HPP

#include <cm_base/templates/relset.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>

using namespace std;

// template syntax brain hurty
template <typename List, typename Delta>
relset<List, Delta>::relset(u32 N, std::function<Delta(List&, u32, u32)> dfunc,
                            bool symmetric) {
    this->N = N;
    this->delfunc = dfunc;
    this->symmetric = symmetric;
    if (symmetric)
        this->dists = std::vector<pair_dist<Delta> >(N * (N - 1) / 2);
    else
        this->dists = std::vector<pair_dist<Delta> >(N * (N - 1));
}

template <typename List, typename Delta>
void relset<List, Delta>::fill_dists(List x) {
    u32 i, j, count = 0;
    for (i = 0; i < this->N; i++) {
        for (j = (this->symmetric ? i + 1 : 0); j < this->N; j++) {
            if (j == i) continue;
            this->dists[count].first = i;
            this->dists[count].second = j;
            this->dists[count].dist = this->delfunc(x, i, j);
            count++;
        }
    }
    std::sort(this->dists.begin(), this->dists.end());
}

template <typename List, typename Delta>
void relset<List, Delta>::disp() {
    for (u32 i = 0; i < this->dists.size(); i++) {
        cout << this->dists[i].first << " " << this->dists[i].second << " "
             << this->dists[i].dist << "\n";
    }
}

/*
template <typename List, typename Delta>
void relset<List, Delta>::disp(const List x) {
    for (u32 i = 0; i < this->dists.size(); i++) {
        cout << this->dists[i].first << " (" << x.row(this->dists[i].first)
             << ") " << this->dists[i].second << " ("
             << x.row(this->dists[i].second) << ") "
             << " " << this->dists[i].dist << "\n";
    }
}


template <typename List, typename Delta>
void relset<List, Delta>::disp2(const List x) {
    for (u32 i = 0; i < this->dists.size(); i++) {
        cout << this->dists[i].first << " (" << x[this->dists[i].first] << ") "
             << this->dists[i].second << " (" << x[this->dists[i].second]
             << ") "
             << " " << this->dists[i].dist << "\n";
    }
}
*/

template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
std::vector<std::set<u32> > edges_from_relsets(u32& n_vert, u32& n_edges,
                                               relset<List1, Delta1>& s1,
                                               relset<List2, Delta2>& s2,
                                               const EpsType epsilon) {
    u32 M = s1.N, N = s2.N;
    u32 i, j;
    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0) {
        throw std::runtime_error(
            "One of the sets is empty (initialization error)\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
    }

    std::vector<std::set<u32> > Edges(n_vert + 1);

    u32 v1, v2;

    auto base = s2.dists.data();
    u32 len1 = s1.symmetric ? M * (M - 1) / 2 : M * (M - 1);
    u32 len2 = s2.symmetric ? N * (N - 1) / 2 : N * (N - 1);

    EpsType cur_ub = 0, cur_lb = 0;
    u32 ub_loc = len2, lb_loc = 0;

    short found1, found2;

    for (i = 0; i < len1; i++) {
        cur_lb = s1.dists[i].dist - epsilon;
        cur_ub = cur_lb + 2 * epsilon;

        found1 = binary_find2(base, len2, cur_lb, lb_loc);
        if (found1 == -1) break;
        found2 = binary_find2(base, len2, cur_ub, ub_loc);
        if (found2 == -1) ub_loc = len2 - 1;

        if (lb_loc > ub_loc) {
            cerr << "Overflow glitch?!?!\n";
            break;
        }

        for (j = lb_loc; j <= ub_loc; j++) {
            // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
            v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

            Edges[v1].insert(v2);
            Edges[v2].insert(v1);
            n_edges++;

            if (!s1.symmetric && !s2.symmetric) continue;
            // if d(i,j) approx= d(i',j') then edge between (i,j') and (j,i')
            v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

            Edges[v1].insert(v2);
            Edges[v2].insert(v1);
            n_edges++;
        }
    }

    return Edges;
}

template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
std::vector<std::set<u32> > efr_condition(
    u32& n_vert, u32& n_edges, relset<List1, Delta1>& s1,
    relset<List2, Delta2>& s2, const EpsType epsilon,
    std::function<bool(u32, u32, u32, u32)> cfunc, bool use_cfunc_only) {
    u32 M = s1.N, N = s2.N;
    u32 i, j;
    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0) {
        throw std::runtime_error(
            "One of the sets is empty (initialization error)\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
    }

    std::vector<std::set<u32> > Edges(n_vert + 1);

    u32 v1, v2;

    auto base = s2.dists.data();
    u32 len1 = s1.symmetric ? M * (M - 1) / 2 : M * (M - 1);
    u32 len2 = s2.symmetric ? N * (N - 1) / 2 : N * (N - 1);

    EpsType cur_ub = 0, cur_lb = 0;
    u32 ub_loc = len2, lb_loc = 0;

    short found1, found2;

    for (i = 0; i < len1; i++) {
        cur_lb = s1.dists[i].dist - epsilon;
        cur_ub = cur_lb + 2 * epsilon;

        if (!use_cfunc_only) {
            found1 = binary_find2(base, len2, cur_lb, lb_loc);
            if (found1 == -1) break;
            found2 = binary_find2(base, len2, cur_ub, ub_loc);
            if (found2 == -1) ub_loc = len2 - 1;

            if (lb_loc > ub_loc) {
                cerr << "Overflow glitch?!?!\n";
                break;
            }
        }

        else {
            lb_loc = 0;
            ub_loc = len2 - 1;
        }

        for (j = lb_loc; j <= ub_loc; j++) {
            // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
            v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

            /*            cout << i << " ( " << s1.dists[i].first << " " <<
               s1.dists[i].second
                             << "\n"
                             << j << " ( " << s2.dists[j].first << " " <<
               s2.dists[j].second
                             << "\n";
                             */
            if (cfunc(s1.dists[i].first, s1.dists[i].second, s2.dists[j].first,
                      s2.dists[j].second)) {
                Edges[v1].insert(v2);
                Edges[v2].insert(v1);
                n_edges++;
            }

            if (!s1.symmetric && !s2.symmetric) continue;
            // if d(i,j) approx= d(i',j') then edge between (i,j') and (j,i')
            v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

            if (s1.symmetric) {
                if (cfunc(s1.dists[i].second, s1.dists[i].first,
                          s2.dists[j].first, s2.dists[j].second)) {
                    Edges[v1].insert(v2);
                    Edges[v2].insert(v1);
                    n_edges++;
                }
            }

            else {
                // s2 has to be symmetric
                if (cfunc(s1.dists[i].first, s1.dists[i].second,
                          s2.dists[i].second, s2.dists[i].first)) {
                    Edges[v1].insert(v2);
                    Edges[v2].insert(v1);
                    n_edges++;
                }
            }
        }
    }

    return Edges;
}

#endif /* RELSET_HPP */

