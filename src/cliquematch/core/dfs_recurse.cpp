#include <core/graph.h>
#include <algorithm>
#include <iostream>

using namespace std;

void graph::dfs_one_search(size_t cur, const graphBits& prev_cand,
                           const graphBits& prev_res)
{
    size_t candidates_left = prev_cand.count();
    size_t mcs_potential = candidates_left + prev_res.count();

#if BENCHMARKING == 0
    if (this->elapsed_time() > this->TIME_LIMIT) return;
#endif
    if (CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT) return;
    if (mcs_potential <= this->CUR_MAX_CLIQUE_SIZE) return;

    // no candidates left => clique cannot grow
    // therefore mcs_potential is same as mcs
    if (candidates_left == 0)
    {
        this->vertices[cur].bits = prev_res;
        this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
        this->vertices[cur].mcs = mcs_potential;
        this->CUR_MAX_CLIQUE_LOC = cur;
        return;
    }

    // There exist candidates for possible clique expansion,
    // so go through them recursively
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
        cand.reset(i);

        // clear the list of future candidates so that we can try with a
        // new set
        future_cand.clear();

        // assume vert is part of the clique
        res.set(i);

        // Check if vert has any common neighbors to cur
        // (apart from each other)
        for (k = 0; k < this->vertices[vert].N; k++)
        {
            f = this->find_if_neighbors(
                cur, this->edge_list[this->vertices[vert].elo + k], ans);
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
        dfs_one_search(cur, future_cand, res);

        // we need to remove vert from clique consideration (undo
        // assumption) because the next (future) candidate may not have
        // it as part of the clique
        res.reset(i);
    }
}

#if STACK_DFS == 0
#pragma message("Using recursion for DFS")
void graph::dfs_one_clique(size_t cur)
{
    graphBits res(this->vertices[cur].N);
    graphBits cand(this->vertices[cur].N);
    std::size_t i, vert;
    res.set(this->vertices[cur].spos);
    // only search thru neighbors with greater degrees
    // (this amortizes the search cost because
    // vertices with really low degree have fewer neighbors
    // and vertices with really high degree will have been counted
    // by someone else)
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        vert = this->edge_list[this->vertices[cur].elo + i];
        if (vert == cur || this->vertices[vert].N < this->vertices[cur].N) continue;
        cand.set(i);
    }
    dfs_one_search(cur, cand, res);
}
#endif

