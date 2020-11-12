#include <detail/heuristic.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    std::size_t DegreeHeuristic::process_graph(graph& G)
    {
        std::size_t i;
        neighbors.resize(G.max_degree);

        request_size =
            (G.max_degree % BITS_PER_SIZE_T != 0) + G.max_degree / BITS_PER_SIZE_T;
        graphBits res(G.load_memory(request_size), G.max_degree);
        graphBits cand(G.load_memory(request_size), G.max_degree);

        process_vertex(G, G.md_vert, res, cand);
        for (i = 0; i < G.n_vert && G.CUR_MAX_CLIQUE_SIZE < G.CLIQUE_LIMIT; i++)
        {
            if (G.vertices[i].mcs <= G.CUR_MAX_CLIQUE_SIZE || i == G.md_vert) continue;
            process_vertex(G, i, res, cand);
        }
        G.clear_memory(2 * request_size);
        return i;
    }

    void DegreeHeuristic::process_vertex(graph& G, std::size_t cur, graphBits& res,
                                         graphBits& cand)
    {
        // heuristic assumes that higher degree neighbors are
        // more likely to be part of a clique
        // so it goes through them in O(N^2) to find a clique
        // (dfs is exponential complexity)
        res.clear(G.vertices[cur].N);
        cand.clear(G.vertices[cur].N);
        res.set(G.vertices[cur].spos);

        std::size_t ans;
        std::size_t i, j;
        candidates_left = 0;
        cur_clique_size = 1;

        // find all neighbors of cur and sort by decreasing degree
        for (i = 0, j = 0; i < G.vertices[cur].spos; i++)
        {
            neighbors[j].load(G.edge_list[G.vertices[cur].elo + i], 0, i);
            if (G.vertices[neighbors[j].id].N <= G.vertices[cur].N) continue;
            neighbors[j].N = G.vertices[neighbors[j].id].N;
            j++;
            cand.set(i);
            candidates_left++;
        }
        for (i = G.vertices[cur].spos + 1; i < G.vertices[cur].N; i++)
        {
            neighbors[j].load(G.edge_list[G.vertices[cur].elo + i], 0, i);
            if (G.vertices[neighbors[j].id].N < G.vertices[cur].N) continue;
            neighbors[j].N = G.vertices[neighbors[j].id].N;
            j++;
            cand.set(i);
            candidates_left++;
        }
        if (candidates_left <= G.CUR_MAX_CLIQUE_SIZE) return;
        std::sort(neighbors.begin(), neighbors.begin() + candidates_left,
                  std::greater<vtriple>());

        cand_max = candidates_left;
        // let neib be a high-degree neighbor of cur that hasn't been searched earlier
        for (i = 0; i < cand_max; i++)
        {
            // should neib be considered as a candidate?
            if (!cand[neighbors[i].pos]) continue;

            // it can be part of the current clique
            res.set(neighbors[i].pos);
            cur_clique_size++;
            cand.reset(neighbors[i].pos);
            candidates_left--;

            // assume neib is a worthwhile candidate
            // modify candidate list using neib's neighbors
            for (j = i + 1; j < cand_max; j++)
            {
                if (G.find_if_neighbors(neighbors[j].id, neighbors[i].id, ans) == 1)
                    continue;
                else
                {
                    candidates_left -= cand[neighbors[j].pos];
                    cand.reset(neighbors[j].pos);
                }
            }

            clique_potential = cur_clique_size + candidates_left;

            if (clique_potential <= G.CUR_MAX_CLIQUE_SIZE)
            {
                // heuristic assumption was not useful, because
                // potential clique with neib cannot beat the maximum
                break;
            }
            else if (candidates_left == 0)
            {
                // there are no candidates left =>
                // potential has been realized and beaten the current maximum
                // so save the clique's data as the new global maximum
                G.vertices[cur].mcs = cur_clique_size;
                G.CUR_MAX_CLIQUE_SIZE = cur_clique_size;
                G.CUR_MAX_CLIQUE_LOC = cur;
                G.vertices[cur].bits.copy_data(res);
                break;
            }
            // else, this clique still has potential to beat the maximum, and
            // some candidates left to try, so continue on with the loop
        }
    }
}  // namespace detail
}  // namespace cliquematch