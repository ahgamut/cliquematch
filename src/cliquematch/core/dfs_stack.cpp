#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <stack>

using namespace std;

#ifdef STACK_DFS
#pragma message("Using explicit stack for DFS")

void graph::dfs_one_clique(std::size_t cur)
{
    std::stack<SearchState> states;
    std::size_t candidates_left, mcs_potential;
    std::size_t vert, ans;

    states.push(SearchState(this->vertices[cur]));
    while (!states.empty())
    {
        if (this->CUR_MAX_CLIQUE_SIZE > this->CLIQUE_LIMIT) return;
        if ((clock() - duration) / CLOCKS_PER_SEC > this->TIME_LIMIT) return;

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
                std::size_t i, k;
                short f = 0;
                for (i = cur_state.start_at; i < this->vertices[cur].N; i++)
                {
                    // keep going until a candidate exists
                    if (cur_state.cand.block_empty(i))
                    {
                        i += (31 - i % 32);
                        continue;
                    }
                    if (!cur_state.cand[i]) continue;

                    // offset thru the edge list to get the neighbor vertex
                    vert = this->edge_list[this->vertices[cur].elo + i];
                    cur_state.cand.reset(i);

                    // only search thru neighbors with greater degrees
                    if (this->vertices[vert].N < this->vertices[cur].N || vert == cur)
                        continue;
                    // (this amortizes the search cost because
                    // vertices with really low degree have fewer neighbors
                    // and vertices with really high degree will have been
                    // counted by someone else)

                    // clear the list of future candidates so that we can try
                    // with a new set
                    future_state.cand.clear();

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
                    // since you just checked the candidacy of vert,
                    // you want to avoid checking it again
                    future_state.cand.reset(i);

                    // you also want to avoid checking cur itself in the
                    // recursive call
                    future_state.cand.reset(this->vertices[cur].spos);

                    // next time we return to this state, we can start searching
                    // using ONLY the vertices after vert, because vert will get
                    // covered by future_state and its descendants
                    cur_state.start_at = i + 1;

                    states.push(future_state);
                    break;
                }
            }
        }
        else
        {
            if (mcs_potential == this->CUR_MAX_CLIQUE_SIZE)
            {
                // so this subtree has the "potential" to TIE with the current
                // maximum but I don't want to waste the time searching for it
                // I'll just mark it for now, and check later
                this->vertices[cur].mcs = mcs_potential;
            }

            // this subtree cannot beat the current maximum clique
            states.pop();
        }
    }
}

#endif
