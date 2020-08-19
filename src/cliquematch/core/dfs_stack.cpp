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
        this->clique_potential = 1;
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (vert == cur || G.vertices[vert].N < G.vertices[cur].N ||
                (G.vertices[vert].N == G.vertices[cur].N && vert < cur))
                continue;
            x.cand.set(j);
            this->clique_potential++;
        }
        if (this->clique_potential <= G.CUR_MAX_CLIQUE_SIZE) return;

        states.push_back(std::move(x));
        clique_size = 1;
        while (!states.empty())
        {
            if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) return;
#if BENCHMARKING == 0
            if (G.elapsed_time() > this->TIME_LIMIT) return;
#endif
            SearchState& cur_state = states.back();
            candidates_left = cur_state.cand.count();
            clique_potential = candidates_left + clique_size;

            for (j = cur_state.start_at; j < G.vertices[cur].N; j++)
            {
                if (!cur_state.cand[j]) continue;
                vert = G.edge_list[G.vertices[cur].elo + j];
                cur_state.cand.reset(j);
                cur_state.start_at = j + 1;

                to_remove.clear();
                for (k = j + 1;
                     k < G.vertices[cur].N && clique_potential > G.CUR_MAX_CLIQUE_SIZE;
                     k++)
                {
                    if (!cur_state.cand[k]) continue;
                    f = G.find_if_neighbors(vert, G.edge_list[G.vertices[cur].elo + k],
                                            ans);
                    if (f != 1) to_remove.emplace_back(k);
                    f = 0;
                    clique_potential =
                        (candidates_left - to_remove.size()) + clique_size;
                }

                if (clique_potential > G.CUR_MAX_CLIQUE_SIZE)
                {
                    if (candidates_left == to_remove.size() + 1)
                    {
                        cur_state.res.set(j);
                        G.vertices[cur].bits.copy_data(cur_state.res);
                        G.vertices[cur].mcs = clique_potential;
                        G.CUR_MAX_CLIQUE_SIZE = clique_potential;
                        G.CUR_MAX_CLIQUE_LOC = cur;
                        cur_state.res.reset(j);
                    }

                    else
                    {
                        SearchState future_state;
                        future_state.refer_from(cur_state.cand, cur_state.res, j);
                        for (auto k : to_remove) future_state.cand.reset(k);
                        states.push_back(std::move(future_state));
                        clique_size++;
                        break;
                    }
                }
            }
            if (j == G.vertices[cur].N)
            {
                cur_state.res.reset(cur_state.id);
                states.pop_back();
                clique_size--;
            }
        }
    }
#endif
}  // namespace detail
}  // namespace cliquematch
