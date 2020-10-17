#include <detail/dfs.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    std::size_t RecursionDFS::process_graph(graph& G)
    {
        std::size_t i = start_vertex;
        G.clear_memory(G.search_cur - G.search_start);
        for (; i < G.n_vert; i++)
        {
            if (G.vertices[i].mcs <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
            if (G.elapsed_time() > this->TIME_LIMIT) break;
            process_vertex(G, i);
            G.clear_memory(2 * request_size);
        }
        // If we pause midway, I want to know where we stopped
        return i;
    }

    void RecursionDFS::process_vertex(graph& G, std::size_t cur)
    {
        request_size = (G.vertices[cur].N % BITS_PER_SIZE_T != 0) +
                       G.vertices[cur].N / BITS_PER_SIZE_T;
        graphBits res(G.load_memory(request_size), G.vertices[cur].N, false);
        graphBits cand(G.load_memory(request_size), G.vertices[cur].N, false);
        res.set(G.vertices[cur].spos);
        std::size_t j, vert, clique_potential = 1;
        // only search thru neighbors with greater degrees
        // (this amortizes the search cost because
        // vertices with really low degree have fewer neighbors
        // and vertices with really high degree will have been counted
        // by someone else)
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (vert == cur || G.vertices[vert].N < G.vertices[cur].N ||
                (G.vertices[vert].N == G.vertices[cur].N && vert < cur))
                continue;
            cand.set(j);
            clique_potential++;
        }
        if (clique_potential <= G.CUR_MAX_CLIQUE_SIZE) return;

        search_vertex(G, cur, cand, res);
    }

    void RecursionDFS::search_vertex(graph& G, std::size_t cur,
                                     const graphBits& prev_cand, graphBits& res)
    {
        std::size_t candidates_left = prev_cand.count();
        std::size_t clique_potential = candidates_left + res.count();

        if (G.elapsed_time() > this->TIME_LIMIT) return;
        if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) return;
        if (clique_potential <= G.CUR_MAX_CLIQUE_SIZE) return;

        // no candidates left => clique cannot grow
        // therefore clique_potential is same as mcs
        if (candidates_left == 0)
        {
            G.vertices[cur].bits.copy_data(res);
            G.vertices[cur].mcs = clique_potential;
            G.CUR_MAX_CLIQUE_SIZE = clique_potential;
            G.CUR_MAX_CLIQUE_LOC = cur;
            return;
        }

        // There exist candidates for possible clique expansion,
        // so go through them recursively
        graphBits cand, future_cand;
        cand.copy_from(prev_cand, G.load_memory(request_size));
        future_cand.copy_from(prev_cand, G.load_memory(request_size));

        std::size_t j, k, vert, ans;
        short f = 0;
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            // keep going until a candidate exists
            if (cand.block_empty(j))
            {
                j += (31 - j % 32);
                continue;
            }
            if (!cand[j]) continue;

            // offset thru the edge list to get the neighbor vertex
            vert = G.edge_list[G.vertices[cur].elo + j];
            // assume vert is part of the clique
            res.set(j);
            cand.reset(j);
            future_cand.copy_data(cand);

            // Check if the remaining candidates in cur are neighbors to vert
            for (k = j + 1; k < G.vertices[cur].N; k++)
            {
                if (future_cand.block_empty(k))
                {
                    k += (31 - k % 32);
                    continue;
                }
                if (!future_cand[k]) continue;
                f = G.find_if_neighbors(vert, G.edge_list[G.vertices[cur].elo + k],
                                        ans);
                if (f != 1)
                {
                    future_cand.reset(k);
                    f = 0;
                }
            }

            search_vertex(G, cur, future_cand, res);
            // we need to remove vert from clique consideration (undo
            // assumption) because the next (future) candidate may not have
            // it as part of the clique
            res.reset(j);
        }
        G.clear_memory(2 * request_size);
    }
}  // namespace detail
}  // namespace cliquematch
