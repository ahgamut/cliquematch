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
    std::stack<SearchState> states;
    std::size_t candidates_left, mcs_potential;
    std::size_t vert, ans;
    std::size_t i, k;
    short f = 0;
    SearchState x(this->vertices[cur]);
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        vert = this->edge_list[this->vertices[cur].elo + i];
        if (vert == cur || this->vertices[vert].N < this->vertices[cur].N) continue;
        x.cand.set(i);
    }
    states.push(x);
    while (!states.empty())
    {
        if (this->CUR_MAX_CLIQUE_SIZE > this->CLIQUE_LIMIT) return;
#if BENCHMARKING == 0
        if (this->elapsed_time() > this->TIME_LIMIT) return;
#endif
        SearchState& cur_state = states.top();
        candidates_left = cur_state.cand.count();
        mcs_potential = candidates_left + cur_state.res.count();
        /*
                std::cout << cur << " " << candidates_left << ":" << mcs_potential << "
           "; cur_state.res.show(&this->edge_list[this->vertices[cur].elo],
                                   this->vertices[cur].N);*/
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
                    cur_state.cand.reset(i);

                    // assume vert is part of the clique
                    future_state.res.set(i);

                    // Check if vert has any common neighbors to cur
                    // (apart from each other)
                    for (k = 0; k < this->vertices[vert].N; k++)
                    {
                        f = this->find_if_neighbors(
                            this->vertices[cur],
                            this->edge_list[this->vertices[vert].elo + k], ans);
                        // break if no more common neighbors
                        if (f == -1) break;

                        // if there is a common neighbor,
                        // add it to the list of candidates for the recursive
                        // call
                        else if (f == 1 && cur_state.cand[ans])
                        {
                            future_state.cand.set(ans);
                            // cerr<<this->edge_list[this->vertices[cur].elo+ans]<<"@"<<ans<<"\n";
                            f = 0;
                        }
                    }
                    future_state.cand.reset(i);
                    // next time we return to this state, we can start searching
                    // using ONLY the vertices after vert, because vert will get
                    // covered by future_state and its descendants
                    cur_state.res.reset(i);
                    states.push(future_state);
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
