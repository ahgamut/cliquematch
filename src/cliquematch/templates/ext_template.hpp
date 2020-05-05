#ifndef EXT_TEMPLATE_HPP
#define EXT_TEMPLATE_HPP

#include <templates/ext_template.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;
// template syntax brain hurty

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::GraphTemplate()
{
    this->nvert = 0;
    this->nedges = 0;
    this->use_heur = false;
    this->use_dfs = false;
    this->lower_bound = 0;
    this->upper_bound = 32;
    this->time_lim = 100;
    this->finished_heur = false;
    this->finished_all = false;
    this->current_vertex = 0;
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::GraphTemplate(
    List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
    std::function<Delta1(List1&, std::size_t, std::size_t)> d1, bool is_d1_symmetric,
    std::function<Delta2(List2&, std::size_t, std::size_t)> d2, bool is_d2_symmetric)
    : GraphTemplate<List1, List2, Delta1, Delta2, EpsType>()
{
    this->ps1 = relset<List1, Delta1>(pts1_len, d1, is_d1_symmetric);
    this->ps2 = relset<List2, Delta2>(pts2_len, d2, is_d2_symmetric);
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
bool GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::build_edges(List1& pts1,
                                                                       List2& pts2)
{
    this->ps1.fill_dists(pts1);
    this->ps2.fill_dists(pts2);
    std::size_t no_of_vertices, no_of_edges;
    auto edges = edges_from_relsets(no_of_vertices, no_of_edges, this->ps1, this->ps2,
                                    this->pts_epsilon);
    if (edges.data() == nullptr || edges.size() == 0)
        throw CM_ERROR("Could not extract edges");

    this->load_graph(no_of_vertices, no_of_edges, edges);
    return true;
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
bool GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::build_edges_with_condition(
    List1& pts1, List2& pts2,
    std::function<bool(List1&, std::size_t, std::size_t, List2&, std::size_t,
                       std::size_t)>
        cfunc,
    bool use_cfunc_only)
{
    std::size_t no_of_vertices, no_of_edges;
    auto cfwrap = [&pts1, &pts2, &cfunc](std::size_t i, std::size_t j, std::size_t i2,
                                         std::size_t j2) -> bool {
        return cfunc(pts1, i, j, pts2, i2, j2);
    };

    if (!use_cfunc_only)
    {
        this->ps1.fill_dists(pts1);
        this->ps2.fill_dists(pts2);
    }
    auto edges = efr_condition(no_of_vertices, no_of_edges, this->ps1, this->ps2,
                               this->pts_epsilon, cfwrap, use_cfunc_only);
    if (edges.data() == nullptr || edges.size() == 0)
        throw CM_ERROR("Could not extract edges");

    this->load_graph(no_of_vertices, no_of_edges, edges);
    return true;
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
py::list GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::get_correspondence2(
    std::vector<std::size_t> clique)
{
    py::list a1;
    py::list a2;

    std::size_t i, t1, t2;
    for (i = 0; i < clique.size(); i++)
    {
        if (clique[i] == 0)
        {
            throw CM_ERROR(
                "Sentinel Value (0) appeared in clique. Increase the time "
                "limit; if repeated, reset graph\n");
        }
        t1 = (clique[i] - 1) / ps2.N;
        t2 = (clique[i] - 1) % ps2.N;
        a1.append(t1);
        a2.append(t2);
    }
    py::list ans;

    ans.append(a1);
    ans.append(a2);

    return ans;
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
py::list GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::get_correspondence()
{
    return this->get_correspondence2(this->get_max_clique());
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
std::string GraphTemplate<List1, List2, Delta1, Delta2, EpsType>::showdata()
{
    std::stringstream ss;
    ss << "cliquematch.core.<templated class object> at " << this << "\n(";
    ss << "epsilon=" << this->pts_epsilon << ",";
    ss << "n_vertices=" << this->nvert << ",";
    ss << "n_edges=" << this->nedges << ",";
    ss << "search_done=" << (this->finished_all ? "True" : "False");
    ss << "lower_bound=" << this->lower_bound << ",";
    ss << "upper_bound=" << this->upper_bound << ",";
    ss << "time_limit=" << this->time_lim << ",";
    ss << "use_heuristic=" << (this->use_heur ? "True" : "False") << ",";
    ss << "use_dfs=" << (this->use_dfs ? "True" : "False") << ",";
    ss << ")";

    return ss.str();
}

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
std::vector<std::set<std::size_t> > edges_from_relsets(std::size_t& n_vert,
                                                       std::size_t& n_edges,
                                                       const relset<List1, Delta1>& s1,
                                                       const relset<List2, Delta2>& s2,
                                                       const EpsType epsilon)
{
    std::size_t M = s1.N, N = s2.N;
    std::size_t i, j;
    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0)
    {
        throw std::runtime_error("One of the sets is empty (initialization error)\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    std::vector<std::set<std::size_t> > Edges(n_vert + 1);

    std::size_t v1, v2;

    auto base = s2.dists.data();
    std::size_t len1 = s1.symmetric ? M * (M - 1) / 2 : M * (M - 1);
    std::size_t len2 = s2.symmetric ? N * (N - 1) / 2 : N * (N - 1);

    EpsType cur_ub = 0, cur_lb = 0;
    std::size_t ub_loc = len2, lb_loc = 0;

    short found1, found2;

    for (i = 0; i < len1; i++)
    {
        cur_lb = s1.dists[i].dist - epsilon;
        cur_ub = cur_lb + 2 * epsilon;

        found1 = binary_find2(base, len2, cur_lb, lb_loc);
        if (found1 == -1) break;
        found2 = binary_find2(base, len2, cur_ub, ub_loc);
        if (found2 == -1) ub_loc = len2 - 1;

        if (lb_loc > ub_loc)
        {
            cerr << "Overflow glitch?!?!\n";
            break;
        }

        for (j = lb_loc; j <= ub_loc; j++)
        {
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

template <typename List1, typename List2, typename Delta1, typename Delta2,
          typename EpsType>
std::vector<std::set<std::size_t> > efr_condition(
    std::size_t& n_vert, std::size_t& n_edges, const relset<List1, Delta1>& s1,
    const relset<List2, Delta2>& s2, const EpsType epsilon,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> cfunc,
    bool use_cfunc_only)
{
    std::size_t M = s1.N, N = s2.N;
    std::size_t i, j;
    n_vert = M * N;
    n_edges = 0;

    if (M == 0 || N == 0)
    {
        throw std::runtime_error("One of the sets is empty (initialization error)\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    std::vector<std::set<std::size_t> > Edges(n_vert + 1);

    std::size_t v1, v2;

    auto base = s2.dists.data();
    std::size_t len1 = s1.symmetric ? M * (M - 1) / 2 : M * (M - 1);
    std::size_t len2 = s2.symmetric ? N * (N - 1) / 2 : N * (N - 1);

    EpsType cur_ub = 0, cur_lb = 0;
    std::size_t ub_loc = len2, lb_loc = 0;

    short found1, found2;

    for (i = 0; i < len1; i++)
    {
        cur_lb = s1.dists[i].dist - epsilon;
        cur_ub = cur_lb + 2 * epsilon;

        if (!use_cfunc_only)
        {
            found1 = binary_find2(base, len2, cur_lb, lb_loc);
            if (found1 == -1) break;
            found2 = binary_find2(base, len2, cur_ub, ub_loc);
            if (found2 == -1) ub_loc = len2 - 1;

            if (lb_loc > ub_loc)
            {
                cerr << "Overflow glitch?!?!\n";
                break;
            }
        }

        else
        {
            lb_loc = 0;
            ub_loc = len2 - 1;
        }

        for (j = lb_loc; j <= ub_loc; j++)
        {
            // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
            v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

            if (cfunc(s1.dists[i].first, s1.dists[i].second, s2.dists[j].first,
                      s2.dists[j].second))
            {
                Edges[v1].insert(v2);
                Edges[v2].insert(v1);
                n_edges++;
            }

            if (!s1.symmetric && !s2.symmetric) continue;
            // if d(i,j) approx= d(i',j') then edge between (i,j') and (j,i')
            v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
            v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

            if (s1.symmetric)
            {
                if (cfunc(s1.dists[i].second, s1.dists[i].first, s2.dists[j].first,
                          s2.dists[j].second))
                {
                    Edges[v1].insert(v2);
                    Edges[v2].insert(v1);
                    n_edges++;
                }
            }

            else
            {
                // s2 has to be symmetric
                if (cfunc(s1.dists[i].first, s1.dists[i].second, s2.dists[j].second,
                          s2.dists[j].first))
                {
                    Edges[v1].insert(v2);
                    Edges[v2].insert(v1);
                    n_edges++;
                }
            }
        }
    }

    return Edges;
}

#endif /* EXT_TEMPLATE_HPP */

