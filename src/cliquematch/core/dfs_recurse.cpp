#include <core/graph.h>
#include <algorithm>
#include <iostream>
using namespace std;

void graph::dfs_one_search(size_t cur, const graphBits& prev_cand,
                           const graphBits& prev_res)
{
    if (CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT) return;
#if BENCHMARKING == 0
    if (this->elapsed_time() > TIME_LIMIT) return;
#endif
    size_t candidates_left = prev_cand.count();
    size_t mcs_potential = candidates_left + prev_res.count();

    if (mcs_potential > this->CUR_MAX_CLIQUE_SIZE)
    {
        if (candidates_left == 0)
        {
            this->vertices[cur].bits = prev_res;
            this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
            this->vertices[cur].mcs = mcs_potential;
            this->CUR_MAX_CLIQUE_LOC = cur;
            return;
        }

        else
        {
            graphBits cand(prev_cand);
            graphBits res(prev_res);
            graphBits future_cand(this->vertices[cur].N);

            size_t i, k;
            short f = 0;
            size_t vert, ans;
            for (i = 0; i < this->vertices[cur].N; i++)
            {
                // keep going until a candidate exists
                if (cand.block_empty(i))
                {
                    i += (31 - i % 32);
                    continue;
                }
                if (!cand[i]) continue;

                // offset thru the edge list to get the neighbor vertex
                vert = this->edge_list[this->vertices[cur].elo + i];

                // assume vert is part of the clique
                res.set(i);
                cand.reset(i);

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
                    // add it to the list of candidates for the recursive call
                    else if (f == 1 && cand[ans])
                    {
                        future_cand.set(ans);
                        // cerr<<this->edge_list[this->vertices[cur].elo+ans]<<"@"<<ans<<"\n";
                        f = 0;
                    }
                }
                future_cand.reset(i);

                dfs_one_search(cur, future_cand, res);

                // we need to remove vert from clique consideration (undo
                // assumption) because the next (future) candidate may not have
                // it as part of the clique
                res.reset(i);
            }
        }
    }
}

#if STACK_DFS == 0
#pragma message("Using recursion for DFS")
void graph::dfs_one_clique(size_t cur)
{
    graphBits res(this->vertices[cur].N);
    graphBits cand(this->vertices[cur].N);
    res.set(this->vertices[cur].spos);
    std::size_t i, vert;
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        vert = this->edge_list[this->vertices[cur].elo + i];
        if (vert == cur || this->vertices[vert].N < this->vertices[cur].N) continue;
        cand.set(i);
    }
    dfs_one_search(cur, cand, res);
}
#endif

