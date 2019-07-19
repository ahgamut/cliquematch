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
    graphBits poss_cand(this->V[cur].N);

    vector<pair<u32, u32> > neib_degs(V[cur].N);

    u32 neib, neib_loc;
    u32 i, k;

    short f, f1, f2;
    u32 ans, count;
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
        f = binary_find(el_base + V[cur].elo, V[cur].N, neib, neib_loc);
        if (!cand[neib_loc]) continue;

        // how good of a candidate is neib ?
        // let's count how many possible candidates it can add;
        // (the number of common neighbors between cur and neib)
        poss_cand.clear();
        count = 0;
        for (k = 0; this->V[neib].N; k++) {
            ans = 0;
            f = binary_find(el_base + V[cur].elo, V[cur].N,
                            el_base[V[neib].elo + k], ans);
            if (f == -1)
                break;
            else if (f == 1) {
                poss_cand.set(ans);
                count++;
                f = 0;
            }
        }

        cand.reset(neib_loc);
        // is neib a worthwhile candidate? (part 1)
        // is the number of edges between neib and cur greater than
        // the current clique size?
        if (count < this->CUR_MAX_CLIQUE_SIZE) {
            res.clear();
            res.set(this->V[cur].spos);
            cand = ~res;
            cur_clique_size = 0;
            break;
        } else {
            if (!res[neib_loc]) cur_clique_size++;
            res.set(neib_loc);
        }

        // I assume neib is a worthwhile candidate
        // will modify candidate list using neib's neighbors
        poss_cand.reset(this->V[cur].spos);
        poss_cand.reset(neib_loc);
        cand = poss_cand;
        candidates_left = cand.count();
        mcs_potential = cur_clique_size + candidates_left;

        if (mcs_potential <= this->CUR_MAX_CLIQUE_SIZE) {
            // heuristic assumption was not useful, because
            // potential clique with neib cannot beat the maximum
            // this vertex is a lost cause. bail
            break;

            // alternatively, I can reset the list of candidates
            // and try with the remaining neighbors
            // (maybe some other time)
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
            break;
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
