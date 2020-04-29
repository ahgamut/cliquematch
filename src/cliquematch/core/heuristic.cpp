#include <core/graph.h>
#include <algorithm>
#include <iostream>
#include <utility>

using namespace std;

bool pair_second_g(const pair<size_t, size_t>& a, const pair<size_t, size_t>& b)
{
    // this can be a lambda
    return (a.second > b.second);
}

void graph::heur_one_clique(size_t cur)
{
    // heuristic assumes that higher degree neighbors are
    // more likely to be part of a clique
    // so it goes through them in O(N^2) to find a clique
    // (dfs is exponential complexity)
    graphBits res(this->vertices[cur].N);
    res.set(this->vertices[cur].spos);
    graphBits cand = ~(res);

    vector<pair<size_t, size_t> > neib_degs(this->vertices[cur].N);

    size_t neib, neib_loc;
    size_t i, j;

    short f1, f2;
    size_t ans1, ans2;
    size_t mcs_potential, candidates_left, cur_clique_size = 1;

    // find all neighbors of cur and sort by decreasing degree
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        neib = this->edge_list[this->vertices[cur].elo + i];
        neib_degs[i] = make_pair(neib, this->vertices[neib].N);
    }
    sort(neib_degs.begin(), neib_degs.end(), pair_second_g);

    // let neib be a high-degree neighbor of cur
    for (i = 0; i < this->vertices[cur].N; i++)
    {
        neib = neib_degs[i].first;
        if (neib == cur) continue;

        // should neib be considered as a candidate?
        neib_loc = 0;
        f1 = this->find_if_neighbors(this->vertices[cur], neib, neib_loc);
        if (this->vertices[neib].N < this->CUR_MAX_CLIQUE_SIZE || !cand[neib_loc])
            continue;

        // it can be part of the current clique
        res.set(neib_loc);
        cand.reset(neib_loc);
        cur_clique_size++;

        // assume neib is a worthwhile candidate
        // modify candidate list using neib's neighbors
        for (j = i + 1; j < this->vertices[cur].N; j++)
        {
            f1 = this->find_if_neighbors(this->vertices[cur], neib_degs[j].first, ans1);
            f2 =
                this->find_if_neighbors(this->vertices[neib], neib_degs[j].first, ans2);
            if (f2 != 1) cand.reset(ans1);
        }

        candidates_left = cand.count();
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
            this->vertices[cur].mcs = mcs_potential;
            this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
            this->CUR_MAX_CLIQUE_LOC = cur;
            this->vertices[cur].bits = res;
            this->vertices[cur].mcs = this->vertices[cur].bits.count();
            // cerr << "Heuristic in " << cur << " updated max_clique to "
            //  << this->vertices[cur].mcs << "\n";
            this->CUR_MAX_CLIQUE_SIZE = this->vertices[cur].mcs;

            break;
        }
        // else, this clique still has potential to beat the maximum, and
        // some candidates left to try, so continue on with the loop
    }
}

size_t graph::heur_all_cliques(size_t start_vertex, double TIME_LIMIT)
{
    size_t i;
    size_t ans;
    for (i = vertices.size() - start_vertex - 1;
         i > 0 && CUR_MAX_CLIQUE_SIZE <= CLIQUE_LIMIT; i--)
    {
#if BENCHMARKING == 0
        if (this->elapsed_time() > TIME_LIMIT) break;
#endif
        if (this->vertices[indices[i]].N <= CUR_MAX_CLIQUE_SIZE) continue;
        if (this->find_if_neighbors(this->vertices[CUR_MAX_CLIQUE_LOC], indices[i],
                                    ans) == 1)
            continue;  // this is very aggressive
        heur_one_clique(indices[i]);
    }
    return vertices.size() - i - 1;
}
