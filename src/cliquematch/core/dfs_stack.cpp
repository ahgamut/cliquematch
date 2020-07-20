#include <core/graph.h>
#include <core/searchstate.h>
#include <algorithm>
#include <iostream>
#include <stack>

using namespace std;

#if STACK_DFS == 1
#pragma message("Using explicit stack for DFS")

void graph::dfs_one_clique(std::size_t cur)
{
    std::size_t candidates_left, mcs_potential = 1;
    std::size_t vert, ans;
    std::size_t i, k;
    SearchState x(this->vertices[cur]);
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        vert = this->edge_list[this->vertices[cur].elo + i];
        if (vert == cur || this->vertices[vert].N < this->vertices[cur].N ||
            (this->vertices[vert].N == this->vertices[cur].N && vert < cur))
            continue;
        x.cand.set(i);
        mcs_potential++;
    }
    if (mcs_potential <= this->CUR_MAX_CLIQUE_SIZE) return;

    short f = 0;
    std::stack<SearchState> states;
    states.push(x);
    while (!states.empty())
    {
        if (this->CUR_MAX_CLIQUE_SIZE >= this->CLIQUE_LIMIT) return;
#if BENCHMARKING == 0
        if (this->elapsed_time() > this->TIME_LIMIT) return;
#endif
        SearchState& cur_state = states.top();
        candidates_left = cur_state.cand.count();
        mcs_potential = candidates_left + cur_state.res.count();

        if (mcs_potential > this->CUR_MAX_CLIQUE_SIZE)
        {
            if (candidates_left == 0)
            {
                this->vertices[cur].bits = cur_state.res;
                this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
                this->vertices[cur].mcs = mcs_potential;
                this->CUR_MAX_CLIQUE_LOC = cur;
                states.pop();
            }

            else
            {
                SearchState future_state(cur_state.cand, cur_state.res);
                for (i = cur_state.start_at; i < this->vertices[cur].N; i++)
                {
                    if (cur_state.cand.block_empty(i))
                    {
                        i += (31 - i % 32);
                        continue;
                    }
                    if (!cur_state.cand[i]) continue;

                    // offset thru the edge list to get the neighbor vertex
                    vert = this->edge_list[this->vertices[cur].elo + i];

                    // assume vert is part of the clique
                    future_state.res.set(i);
                    future_state.cand.reset(i);
                    future_state.start_at = i + 1;

                    // Check if the remaining candidates in cur are neighbors to vert
                    for (k = i + 1; k < this->vertices[cur].N; k++)
                    {
                        if (future_state.cand.block_empty(k))
                        {
                            k += (31 - k % 32);
                            continue;
                        }
                        if (!future_state.cand[k]) continue;
                        f = this->find_if_neighbors(
                            vert, this->edge_list[this->vertices[cur].elo + k], ans);
                        if (f != 1)
                        {
                            future_state.cand.reset(k);
                            f = 0;
                        }
                    }
                    // next time we return to this state, we can start searching
                    // using ONLY the vertices after vert, because vert will get
                    // covered by future_state and its descendants
                    states.push(future_state);
                    cur_state.cand.reset(i);
                    cur_state.res.reset(i);
                    break;
                }
            }
        }
        else
        {
            // this subtree cannot beat the current maximum clique
            states.pop();
        }
    }
}

#endif
