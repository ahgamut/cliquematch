#include <core/graph.h>
#include <algorithm>
#include <iostream>

using namespace std;

struct trip
{
    std::size_t id, N, pos;
    inline bool operator<(const trip& b) const { return (this->N < b.N); }
    inline bool operator>(const trip& b) const { return (this->N > b.N); }
};

void graph::heur_one_clique(size_t cur)
{
    // heuristic assumes that higher degree neighbors are
    // more likely to be part of a clique
    // so it goes through them in O(N^2) to find a clique
    // (dfs is exponential complexity)
    vector<trip> neighbor_trips(this->vertices[cur].N);
    graphBits res(this->vertices[cur].N);
    res.set(this->vertices[cur].spos);
    graphBits cand(this->vertices[cur].N);

    size_t ans;
    size_t i, j;
    size_t mcs_potential, candidates_left, cur_clique_size = 1, cand_max;

    // find all neighbors of cur and sort by decreasing degree
    for (i = 0, j = 0; i < this->vertices[cur].N; i++)
    {
        neighbor_trips[j].id = this->edge_list[this->vertices[cur].elo + i];
        neighbor_trips[j].N = 0;
        neighbor_trips[j].pos = i;
        if (neighbor_trips[j].id != cur &&
            this->vertices[neighbor_trips[j].id].N >= this->vertices[cur].N)
        {
            neighbor_trips[j].N = this->vertices[neighbor_trips[j].id].N;
            cand.set(i);
            j++;
        }
    }
    sort(neighbor_trips.begin(), neighbor_trips.end(), std::greater<trip>());

    cand_max = candidates_left = cand.count();
    // let neib be a high-degree neighbor of cur that hasn't been searched earlier
    for (i = 0; i < cand_max; i++)
    {
        // should neib be considered as a candidate?
        if (!cand[neighbor_trips[i].pos]) continue;

        // it can be part of the current clique
        res.set(neighbor_trips[i].pos);
        cur_clique_size++;
        cand.reset(neighbor_trips[i].pos);
        candidates_left--;

        // assume neib is a worthwhile candidate
        // modify candidate list using neib's neighbors
        for (j = i + 1; j < cand_max; j++)
        {
            if (find_if_neighbors(this->vertices[neighbor_trips[j].id],
                                  neighbor_trips[i].id, ans) == 1)
                continue;
            else
            {
                candidates_left -= cand[neighbor_trips[j].pos];
                cand.reset(neighbor_trips[j].pos);
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
            this->vertices[cur].bits = res;
            // cerr << "Heuristic in " << cur << " updated max_clique to "
            //  << this->vertices[cur].mcs << "\n";

            break;
        }
        // else, this clique still has potential to beat the maximum, and
        // some candidates left to try, so continue on with the loop
    }
}

size_t graph::heur_all_cliques(size_t start_vertex, double TIME_LIMIT)
{
    size_t i;
    for (i = 0; i < vertices.size() && CUR_MAX_CLIQUE_SIZE <= CLIQUE_LIMIT; i++)
    {
#if BENCHMARKING == 0
        if (this->elapsed_time() > TIME_LIMIT) break;
#endif
        if (this->vertices[i].N <= CUR_MAX_CLIQUE_SIZE) continue;
        heur_one_clique(i);
    }
    return i;
}
