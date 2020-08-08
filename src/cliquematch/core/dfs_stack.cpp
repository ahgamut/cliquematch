#include <core/dfs.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
#if STACK_DFS == 1
#pragma message("Using explicit stack for DFS")
    std::size_t StackDFS::process_graph(graph& G, std::size_t start_vertex,
                                        double time_limit)
    {
        this->TIME_LIMIT = time_limit;
        if (!states.empty()) states.clear();
        for (i = start_vertex; i < G.vertices.size(); i++)
        {
            if (G.vertices[i].N <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
#if BENCHMARKING == 0
            if (G.elapsed_time() > TIME_LIMIT) break;
#endif
            process_vertex(G, i);
        }
        // If we pause midway, i want to know where we stopped
        return i;
    }

    void StackDFS::process_vertex(graph& G, std::size_t cur)
    {
        f = 0;
        SearchState x(G.vertices[cur]);

        this->mcs_potential = 1;
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (vert == cur || G.vertices[vert].N < G.vertices[cur].N ||
                (G.vertices[vert].N == G.vertices[cur].N && vert < cur))
                continue;
            x.cand.set(j);
            this->mcs_potential++;
        }
        if (this->mcs_potential <= G.CUR_MAX_CLIQUE_SIZE) return;

        states.push_back(std::move(x));
        while (!states.empty())
        {
            if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) return;
#if BENCHMARKING == 0
            if (G.elapsed_time() > this->TIME_LIMIT) return;
#endif
            SearchState& cur_state = states.back();
            this->candidates_left = cur_state.cand.count();
            this->mcs_potential = candidates_left + cur_state.res.count();

            if (this->mcs_potential > G.CUR_MAX_CLIQUE_SIZE)
            {
                if (this->candidates_left == 0)
                {
                    G.vertices[cur].bits.copy_from(cur_state.res);
                    G.vertices[cur].mcs = mcs_potential;
                    G.CUR_MAX_CLIQUE_SIZE = mcs_potential;
                    G.CUR_MAX_CLIQUE_LOC = cur;
                    states.pop_back();
                }

                else
                {
                    SearchState future_state(cur_state.cand, cur_state.res);
                    for (j = cur_state.start_at; j < G.vertices[cur].N; j++)
                    {
                        if (cur_state.cand.block_empty(j))
                        {
                            j += (31 - j % 32);
                            continue;
                        }
                        if (!cur_state.cand[j]) continue;

                        // offset thru the edge list to get the neighbor vertex
                        vert = G.edge_list[G.vertices[cur].elo + j];

                        // assume vert is part of the clique
                        future_state.res.set(j);
                        future_state.cand.reset(j);
                        future_state.start_at = j + 1;

                        // Check if the remaining candidates in cur are neighbors to
                        // vert
                        for (k = j + 1; k < G.vertices[cur].N; k++)
                        {
                            if (future_state.cand.block_empty(k))
                            {
                                k += (31 - k % 32);
                                continue;
                            }
                            if (!future_state.cand[k]) continue;
                            f = G.find_if_neighbors(
                                vert, G.edge_list[G.vertices[cur].elo + k], ans);
                            if (f != 1)
                            {
                                future_state.cand.reset(k);
                                f = 0;
                            }
                        }
                        // next time we return to this state, we can start searching
                        // using ONLY the vertices after vert, because vert will get
                        // covered by future_state and its descendants
                        cur_state.cand.reset(j);
                        cur_state.res.reset(j);
                        states.push_back(std::move(future_state));
                        break;
                    }
                }
            }
            else
            {
                // this subtree cannot beat the current maximum clique
                states.pop_back();
            }
        }
    }
#endif
}  // namespace detail
}  // namespace cliquematch
