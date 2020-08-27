#ifndef EXT_TEMPLATE_HPP
#define EXT_TEMPLATE_HPP

#include <templates/ext_template.h>

namespace cliquematch
{
namespace ext
{
    // template syntax brain hurty
    namespace py = pybind11;

    template <typename List1, typename List2, typename Delta1, typename Delta2,
              typename EpsType>
    std::vector<std::pair<std::size_t, std::size_t>> edges_from_relsets(
        std::size_t& n_vert, std::size_t& n_edges, const relset<List1, Delta1>& s1,
        const relset<List2, Delta2>& s2, const EpsType epsilon)
    {
        std::size_t M = s1.N, N = s2.N;
        std::size_t i, j;
        n_vert = M * N;
        n_edges = 0;

        if (M == 0 || N == 0)
            throw CM_ERROR("One of the sets is empty (initialization error)\n");

        std::vector<std::pair<std::size_t, std::size_t>> Edges(n_vert + 1);
        for (i = 0; i < Edges.size(); i++) Edges[i] = {i, i};
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

            for (j = lb_loc; j <= ub_loc; j++)
            {
                // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
                v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
                v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

                Edges.push_back(std::make_pair(v1, v2));
                Edges.push_back(std::make_pair(v2, v1));
                n_edges++;

                if (!s1.symmetric && !s2.symmetric) continue;
                // if d(i,j) approx= d(i',j') then edge between (i,j') and (j,i')
                v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
                v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

                Edges.push_back(std::make_pair(v1, v2));
                Edges.push_back(std::make_pair(v2, v1));
                n_edges++;
            }
        }

        return Edges;
    }

    template <typename List1, typename List2, typename Delta1, typename Delta2,
              typename EpsType>
    std::vector<std::pair<std::size_t, std::size_t>> efr_condition(
        std::size_t& n_vert, std::size_t& n_edges, const relset<List1, Delta1>& s1,
        const relset<List2, Delta2>& s2, const EpsType epsilon,
        const std::function<bool(const std::size_t, const std::size_t,
                                 const std::size_t, const std::size_t)>
            cfunc)
    {
        std::size_t M = s1.N, N = s2.N;
        std::size_t i, j;
        n_vert = M * N;
        n_edges = 0;

        if (M == 0 || N == 0)
            throw CM_ERROR("One of the sets is empty (initialization error)\n");

        std::vector<std::pair<std::size_t, std::size_t>> Edges(n_vert + 1);
        for (i = 0; i < Edges.size(); i++) Edges[i] = {i, i};

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

            for (j = lb_loc; j <= ub_loc; j++)
            {
                // if d(i,j) approx= d(i',j') then edge between (i,i') and (j,j')
                v1 = s1.dists[i].first * N + s2.dists[j].first + 1;
                v2 = s1.dists[i].second * N + s2.dists[j].second + 1;

                if (cfunc(s1.dists[i].first, s1.dists[i].second, s2.dists[j].first,
                          s2.dists[j].second))
                {
                    Edges.push_back(std::make_pair(v1, v2));
                    Edges.push_back(std::make_pair(v2, v1));
                    n_edges++;
                }

                if (!s1.symmetric && !s2.symmetric) continue;
                // if d(i,j) approx= d(i',j') then edge between (i,j') and (j,i')
                v1 = s1.dists[i].second * N + s2.dists[j].first + 1;
                v2 = s1.dists[i].first * N + s2.dists[j].second + 1;

                // if either s1 or s2 is symmetric, the alternate mapping is valid
                if (cfunc(s1.dists[i].first, s1.dists[i].second, s2.dists[j].second,
                          s2.dists[j].first))
                {
                    Edges.push_back(std::make_pair(v1, v2));
                    Edges.push_back(std::make_pair(v2, v1));
                    n_edges++;
                }
            }
        }

        return Edges;
    }

    template <typename List1, typename List2, typename Delta1, typename Delta2,
              typename EpsType>
    bool build_edges_metric_only(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len, const EpsType epsilon,
        const std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
            d1,
        const bool is_d1_symmetric,
        const std::function<Delta2(const List2&, const std::size_t, const std::size_t)>
            d2,
        const bool is_d2_symmetric)
    {
        std::size_t no_of_vertices, no_of_edges;
        relset<List1, Delta1> ps1(pts1_len, d1, is_d1_symmetric);
        relset<List2, Delta2> ps2(pts2_len, d2, is_d2_symmetric);
        ps1.fill_dists(pts1);
        ps2.fill_dists(pts2);
        auto edges = edges_from_relsets(no_of_vertices, no_of_edges, ps1, ps2, epsilon);
        if (edges.data() == nullptr || edges.size() == 0)
            throw CM_ERROR("Could not extract edges");
        pg.load_graph(no_of_vertices, no_of_edges, edges);
        return true;
    }

    template <typename List1, typename List2, typename Delta1, typename Delta2,
              typename EpsType>
    bool build_edges_condition_only(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len,
        const std::function<bool(const List1&, const std::size_t, const std::size_t,
                                 const List2&, const std::size_t, const std::size_t)>
            cfunc)
    {
        std::size_t no_of_vertices = pts1_len * pts2_len, no_of_edges = 0;
        std::size_t i1, i2, j1, j2, v1, v2;
        std::vector<std::pair<std::size_t, std::size_t>> edges(no_of_vertices + 1);

        for (v1 = 0; v1 < edges.size(); v1++) edges[v1] = {v1, v1};

        for (i1 = 0; i1 < pts1_len; ++i1)
        {
            for (i2 = i1 + 1; i2 < pts1_len; ++i2)
            {
                for (j1 = 0; j1 < pts2_len; ++j1)
                {
                    for (j2 = j1 + 1; j2 < pts2_len; ++j2)
                    {
                        if (cfunc(pts1, i1, i2, pts2, j1, j2))
                        {
                            v1 = i1 * pts2_len + j1 + 1;
                            v2 = i2 * pts2_len + j2 + 1;
                            edges.push_back(std::make_pair(v1, v2));
                            edges.push_back(std::make_pair(v2, v1));

                            // ouch
                            v1 = i2 * pts2_len + j1 + 1;
                            v2 = i1 * pts2_len + j2 + 1;
                            edges.push_back(std::make_pair(v1, v2));
                            edges.push_back(std::make_pair(v2, v1));

                            no_of_edges += 2;
                        }
                    }
                }
            }
        }
        pg.load_graph(no_of_vertices, no_of_edges, edges);
        return true;
    }

    template <typename List1, typename List2, typename Delta1, typename Delta2,
              typename EpsType>
    bool build_edges(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len, const EpsType epsilon,
        const std::function<bool(const List1&, const std::size_t, const std::size_t,
                                 const List2&, const std::size_t, const std::size_t)>
            cfunc,
        const std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
            d1,
        const bool is_d1_symmetric,
        const std::function<Delta2(const List2&, const std::size_t, const std::size_t)>
            d2,
        const bool is_d2_symmetric)
    {
        std::size_t no_of_vertices, no_of_edges;
        auto cfwrap = [&pts1, &pts2, &cfunc](const std::size_t i, const std::size_t j,
                                             const std::size_t i2,
                                             const std::size_t j2) -> bool {
            return cfunc(pts1, i, j, pts2, i2, j2);
        };
        relset<List1, Delta1> ps1(pts1_len, d1, is_d1_symmetric);
        relset<List2, Delta2> ps2(pts2_len, d2, is_d2_symmetric);
        ps1.fill_dists(pts1);
        ps2.fill_dists(pts2);
        auto edges =
            efr_condition(no_of_vertices, no_of_edges, ps1, ps2, epsilon, cfwrap);
        if (edges.data() == nullptr || edges.size() == 0)
            throw CM_ERROR("Could not extract edges");
        pg.load_graph(no_of_vertices, no_of_edges, edges);

        return true;
    }

}  // namespace ext
}  // namespace cliquematch
#endif /* EXT_TEMPLATE_HPP */

