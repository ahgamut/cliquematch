#include <core/graph.h>
#include <algorithm>
#include <iostream>

using namespace std;
struct vtriple
{
    std::size_t id, N, pos;
    inline bool operator<(const vtriple& b) const { return (this->N < b.N); }
    inline bool operator>(const vtriple& b) const { return (this->N > b.N); }
};

void graph::heur_one_clique(std::size_t cur, vector<vtriple>& neighbors, graphBits& res,
                            graphBits& cand)
{
    // heuristic assumes that higher degree neighbors are
    // more likely to be part of a clique
    // so it goes through them in O(N^2) to find a clique
    // (dfs is exponential complexity)
    res.clear(this->vertices[cur].N);
    cand.clear(this->vertices[cur].N);
    res.set(this->vertices[cur].spos);

    std::size_t ans;
    std::size_t i, j;
    std::size_t mcs_potential, candidates_left = 0, cur_clique_size = 1, cand_max;

    // find all neighbors of cur and sort by decreasing degree
    for (i = 0, j = 0; i < this->vertices[cur].N; i++)
    {
        neighbors[j].id = this->edge_list[this->vertices[cur].elo + i];
        neighbors[j].N = 0;
        neighbors[j].pos = i;
        if (neighbors[j].id != cur &&
            this->vertices[neighbors[j].id].N >= this->vertices[cur].N)
        {
            neighbors[j].N = this->vertices[neighbors[j].id].N;
            cand.set(i);
            j++;
            candidates_left++;
        }
    }
    sort(neighbors.begin(), neighbors.begin() + candidates_left,
         std::greater<vtriple>());

    cand_max = candidates_left;
    // let neib be a high-degree neighbor of cur that hasn't been searched earlier
    for (i = 0; i < cand_max; i++)
    {
        // should neib be considered as a candidate?
        if (!cand[neighbors[i].pos]) continue;

        // it can be part of the current clique
        res.set(neighbors[i].pos);
        cur_clique_size++;
        cand.reset(neighbors[i].pos);
        candidates_left--;

        // assume neib is a worthwhile candidate
        // modify candidate list using neib's neighbors
        for (j = i + 1; j < cand_max; j++)
        {
            if (find_if_neighbors(neighbors[j].id, neighbors[i].id, ans) == 1)
                continue;
            else
            {
                candidates_left -= cand[neighbors[j].pos];
                cand.reset(neighbors[j].pos);
            }
        }

        mcs_potential = cur_clique_size + candidates_left;

        if (mcs_potential <= this->CUR_MAX_CLIQUE_SIZE)
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
            this->vertices[cur].mcs = cur_clique_size;
            this->CUR_MAX_CLIQUE_SIZE = cur_clique_size;
            this->CUR_MAX_CLIQUE_LOC = cur;
            this->vertices[cur].bits |= res;
            // cerr << "Heuristic in " << cur << " updated max_clique to "
            //  << this->vertices[cur].mcs << "\n";

            break;
        }
        // else, this clique still has potential to beat the maximum, and
        // some candidates left to try, so continue on with the loop
    }
}

std::size_t graph::heur_all_cliques(std::size_t start_vertex, double TIME_LIMIT)
{
    std::size_t i;
    vector<vtriple> neibs(this->max_degree);
    graphBits res(this->max_degree);
    graphBits cand(this->max_degree);

    for (i = 0; i < vertices.size() && CUR_MAX_CLIQUE_SIZE < CLIQUE_LIMIT; i++)
    {
#if BENCHMARKING == 0
        if (this->elapsed_time() > TIME_LIMIT) break;
#endif
        if (this->vertices[i].N <= CUR_MAX_CLIQUE_SIZE) continue;
        heur_one_clique(i, neibs, res, cand);
    }
    return i;
}
