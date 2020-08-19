#include <core/dfs.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
#if STACK_DFS == 0
#pragma message("Using recursion for DFS")
    std::size_t RecursionDFS::process_graph(graph& G, std::size_t start_vertex,
                                            double time_limit)
    {
        std::size_t i = start_vertex;
        this->TIME_LIMIT = time_limit;
        for (; i < G.vertices.size(); i++)
        {
            if (G.vertices[i].N <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
#if BENCHMARKING == 0
            if (G.elapsed_time() > this->TIME_LIMIT) break;
#endif
            process_vertex(G, i);
        }
        // If we pause midway, I want to know where we stopped
        return i;
    }

    void RecursionDFS::process_vertex(graph& G, std::size_t cur)
    {
        graphBits res(G.vertices[cur].N);
        graphBits cand(G.vertices[cur].N);
        std::size_t i, vert, mcs_potential = 1;
        res.set(G.vertices[cur].spos);
        // only search thru neighbors with greater degrees
        // (this amortizes the search cost because
        // vertices with really low degree have fewer neighbors
        // and vertices with really high degree will have been counted
        // by someone else)
        for (i = 0; i < G.vertices[cur].N; i++)
        {
            vert = G.edge_list[G.vertices[cur].elo + i];
            if (vert == cur || G.vertices[vert].N < G.vertices[cur].N ||
                (G.vertices[vert].N == G.vertices[cur].N && vert < cur))
                continue;
            cand.set(i);
            mcs_potential++;
        }
        if (mcs_potential > G.CUR_MAX_CLIQUE_SIZE) search_vertex(G, cur, cand, res);
    }

    void RecursionDFS::search_vertex(graph& G, std::size_t cur,
                                     const graphBits& prev_cand, graphBits& res)
    {
        std::size_t candidates_left = prev_cand.count();
        std::size_t mcs_potential = candidates_left + res.count();

#if BENCHMARKING == 0
        if (G.elapsed_time() > this->TIME_LIMIT) return;
#endif
        if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) return;
        if (mcs_potential <= G.CUR_MAX_CLIQUE_SIZE) return;

        // no candidates left => clique cannot grow
        // therefore mcs_potential is same as mcs
        if (candidates_left == 0)
        {
            G.vertices[cur].bits.copy_data(res);
            G.vertices[cur].mcs = mcs_potential;
            G.CUR_MAX_CLIQUE_SIZE = mcs_potential;
            G.CUR_MAX_CLIQUE_LOC = cur;
            return;
        }

        // There exist candidates for possible clique expansion,
        // so go through them recursively
        graphBits cand;
        cand.copy_from(prev_cand);

        std::size_t i, k;
        short f = 0;
        std::size_t vert, ans;
        for (i = 0; i < G.vertices[cur].N; i++)
        {
            // keep going until a candidate exists
            if (cand.block_empty(i))
            {
                i += (31 - i % 32);
                continue;
            }
            if (!cand[i]) continue;

            // offset thru the edge list to get the neighbor vertex
            vert = G.edge_list[G.vertices[cur].elo + i];
            cand.reset(i);

            // assume vert is part of the clique
            res.set(i);
            // Check if the remaining candidates in cur are neighbors to vert
            for (k = i + 1; k < G.vertices[cur].N; k++)
            {
                if (cand.block_empty(k))
                {
                    k += (31 - k % 32);
                    continue;
                }
                if (!cand[k]) continue;
                f = G.find_if_neighbors(vert, G.edge_list[G.vertices[cur].elo + k],
                                        ans);
                if (f != 1)
                {
                    cand.reset(k);
                    f = 0;
                }
            }

            search_vertex(G, cur, cand, res);

            // we need to remove vert from clique consideration (undo
            // assumption) because the next (future) candidate may not have
            // it as part of the clique
            res.reset(i);
        }
    }
#endif
}  // namespace detail
}  // namespace cliquematch
