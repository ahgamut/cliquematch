#include <detail/nwgraph/nwclique.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    u64 NWHeuristic::process_graph(nwgraph& G)
    {
        u64 i;
        neighbors.resize(G.max_neighbors);

        request_size =
            (G.max_neighbors % BITS_PER_U64 != 0) + G.max_neighbors / BITS_PER_U64;
        // "memory" allocations for cand, res
        graphBits res(G.load_memory(request_size), G.max_neighbors);
        graphBits cand(G.load_memory(request_size), G.max_neighbors);

        process_vertex(G, G.CUR_MAX_CLIQUE_LOCATION, res, cand);
        for (i = 0; i < G.n_vert && G.CUR_MAX_CLIQUE_SIZE < G.CLIQUE_LIMIT; i++)
        {
            if (G.vertices[i].mcs <= G.CUR_MAX_CLIQUE_SIZE) continue;
            process_vertex(G, i, res, cand);
        }
        G.clear_memory(2 * request_size);  // release memory
        return i;
    }

    void NWHeuristic::process_vertex(nwgraph& G, u64 cur, graphBits& res,
                                     graphBits& cand)
    {
        // heuristic assumes that higher degree neighbors are
        // more likely to be part of a clique
        // so it goes through them in O(N^2) to find a clique
        // (dfs is exponential complexity)
        res.clear(G.vertices[cur].N);
        cand.clear(G.vertices[cur].N);
        res.set(G.vertices[cur].spos);

        u64 ans;
        u64 i, j;
        candidates_left = 0;
        cand_potential = 0;
        cur_clique_weight = G.vertices[cur].weight;

        // find all neighbors of cur and sort by decreasing degree
        for (i = 0, j = 0; i < G.vertices[cur].spos; i++)
        {
            neighbors[j].load(G.edge_list[G.vertices[cur].elo + i], 0, i, 0);
            if (G.vertices[neighbors[j].id].degree <= G.vertices[cur].degree) continue;
            neighbors[j].N = G.vertices[neighbors[j].id].N;
            neighbors[j].degree = G.vertices[neighbors[j].id].degree;
            j++;
            cand.set(i);
            candidates_left++;
            cand_potential += G.vertices[neighbors[j].id].weight;
        }
        for (i = G.vertices[cur].spos + 1; i < G.vertices[cur].N; i++)
        {
            neighbors[j].load(G.edge_list[G.vertices[cur].elo + i], 0, i, 0);
            if (G.vertices[neighbors[j].id].degree < G.vertices[cur].degree) continue;
            neighbors[j].N = G.vertices[neighbors[j].id].N;
            neighbors[j].degree = G.vertices[neighbors[j].id].degree;
            j++;
            cand.set(i);
            candidates_left++;
            cand_potential += G.vertices[neighbors[j].id].weight;
        }
        if (candidates_left <= G.CUR_MAX_CLIQUE_SIZE) return;
        std::sort(neighbors.begin(), neighbors.begin() + candidates_left,
                  std::greater<vdata>());

        cand_max = candidates_left;
        // let neib be a high-degree neighbor of cur that hasn't been searched earlier
        for (i = 0; i < cand_max; i++)
        {
            // should neib be considered as a candidate?
            if (!cand[neighbors[i].pos]) continue;

            // it can be part of the current clique
            res.set(neighbors[i].pos);
            cur_clique_weight += G.vertices[neighbors[i].id].weight;
            cand.reset(neighbors[i].pos);
            candidates_left--;
            cand_potential -= G.vertices[neighbors[i].id].weight;

            // assume neib is a worthwhile candidate
            // modify candidate list: remove all vertices that are not adjacent to neib
            for (j = i + 1; j < cand_max; j++)
            {
                if (!cand[neighbors[j].pos] ||
                    G.find_if_neighbors(neighbors[j].id, neighbors[i].id, ans) == FOUND)
                    continue;
                else
                {
                    candidates_left--;
                    cand.reset(neighbors[j].pos);
                    cand_potential -= G.vertices[neighbors[j].id].weight;
                }
            }

            clique_potential = cur_clique_weight + cand_potential;

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
                G.CUR_MAX_CLIQUE_SIZE = cur_clique_weight;
                G.CUR_MAX_CLIQUE_LOCATION = cur;
                G.vertices[cur].bits.copy_data(res);
                /* Note that we are not saving cur.mcs because a proper search through
                 * the vertex may give a larger clique */
                break;
            }
            // else, this clique still has potential to beat the maximum, and
            // some candidates left to try, so continue on with the loop
        }
    }
}  // namespace detail
}  // namespace cliquematch
