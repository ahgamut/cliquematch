#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <utility>

using namespace std;

bool pair_second_g(const pair<u32, u32>& a, const pair<u32, u32>& b) {
    // this can be a lambda
    return (a.second > b.second);
}

void graph::heur_one_clique(u32 cur) {
    // heuristic assumes that higher degree neighbors are
    // more likely to be part of a clique
    // so it goes through them in O(N^2) to find a clique
    // (dfs is exponential complexity)
    graphBits res(this->V[cur].N);
    res.set(this->V[cur].spos);
    graphBits cand = ~(res);

    vector<pair<u32, u32> > neib_degs(V[cur].N);

    u32 neib, neib_loc;
    u32 i, j;

    short f1, f2;
    u32 ans1, ans2;
    u32 mcs_potential, candidates_left, cur_clique_size = 1;

    // find all neighbors of cur and sort by decreasing degree
    for (i = 0; i < this->V[cur].N; i++) {
        neib = el_base[V[cur].elo + i];
        neib_degs[i] = make_pair(neib, this->V[neib].N);
    }
    sort(neib_degs.begin(), neib_degs.end(), pair_second_g);

    // let neib be a high-degree neighbor of cur
    for (i = 0; i < this->V[cur].N; i++) {
        neib = neib_degs[i].first;
        if (neib == cur) continue;

        // should neib be considered as a candidate?
        neib_loc = 0;
        f1 = this->find_if_neighbors(V[cur], neib, neib_loc);
        if (this->V[neib].N < this->CUR_MAX_CLIQUE_SIZE || !cand[neib_loc])
            continue;

        // it can be part of the current clique
        res.set(neib_loc);
        cand.reset(neib_loc);
        cur_clique_size++;

        // assume neib is a worthwhile candidate
        // modify candidate list using neib's neighbors
        for (j = i + 1; j < this->V[cur].N; j++) {
            f1 = this->find_if_neighbors(V[cur], neib_degs[j].first, ans1);
            f2 = this->find_if_neighbors(V[neib], neib_degs[j].first, ans2);
            if (f2 != 1) cand.reset(ans1);
        }

        candidates_left = cand.count();
        mcs_potential = cur_clique_size + candidates_left;

        if (mcs_potential <= this->CUR_MAX_CLIQUE_SIZE) {
            // heuristic assumption was not useful, because
            // potential clique with neib cannot beat the maximum
            // Reset the list of candidates
            // and try with the remaining neighbors
            cand = ~res;
            res.clear();
            res.set(this->V[cur].spos);
            cur_clique_size = 1;
        } else if (candidates_left == 0) {
            // there are no candidates left =>
            // potential has been realized and beaten the current maximum
            // so save the clique's data as the new global maximum
            this->V[cur].mcs = mcs_potential;
            this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
            this->CUR_MAX_CLIQUE_LOC = cur;
            this->V[cur].bits = res;
            this->V[cur].mcs = this->V[cur].bits.count();
            cerr << "Heuristic in " << cur << " updated max_clique to "
                 << this->V[cur].mcs << "\n";
            this->CUR_MAX_CLIQUE_SIZE = this->V[cur].mcs;

            // this vertex might have a different set of neighbors who may form
            // a bigger clique
            cand = ~res;
            res.clear();
            res.set(this->V[cur].spos);
            cur_clique_size = 1;
        }
        // else, this clique still has potential to beat the maximum, and
        // some candidates left to try, so continue on with the loop
    }
}

u32 graph::heur_all_cliques(u32 start_vertex, double TIME_LIMIT) {
    u32 i;
    for (i = start_vertex;
         i < vertices.size() && CUR_MAX_CLIQUE_SIZE <= CLIQUE_LIMIT; i++) {
        if (V[i].N <= CUR_MAX_CLIQUE_SIZE) continue;
        if ((clock() - duration) / CLOCKS_PER_SEC > TIME_LIMIT) {
            cerr << "Heuristic: Exceeded time limit of " << TIME_LIMIT
                 << " seconds\n";
            break;
        }
        heur_one_clique(i);
    }
    return i;
}
