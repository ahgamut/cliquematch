#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

void graph::dfs_one_search(u32 cur, const graphBits& prev_cand,
                           const graphBits& prev_res) {
    if (CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT) return;
    if ((clock() - duration) / CLOCKS_PER_SEC > TIME_LIMIT) return;

    u32 candidates_left = prev_cand.count();
    u32 mcs_potential = candidates_left + prev_res.count();

    if (mcs_potential > this->CUR_MAX_CLIQUE_SIZE) {
        // no candidates left => clique cannot grow
        // therefore mcs_potential is same as mcs
        if (candidates_left == 0) {
            this->V[cur].bits = prev_res;
            this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
            this->V[cur].mcs = mcs_potential;
            this->CUR_MAX_CLIQUE_LOC = cur;
            cerr << "\n"
                 << cur << " updated max_clique size to " << CUR_MAX_CLIQUE_SIZE
                 << " ";
            return;
        }

        // There exist candidates for possible clique expansion,
        // so go through them recursively
        else {
            graphBits cand(prev_cand);
            graphBits res(prev_res);
            graphBits future_cand(this->V[cur].N);

            u32 i, k;
            short f = 0;
            u32 vert, ans;
            for (i = 0; i < this->V[cur].N; i++) {
                // keep going until a candidate exists
                if (cand.block_empty(i)) {
                    i += (31 - i % 32);
                    continue;
                }
                if (!cand[i]) continue;

                // offset thru the edge list to get the neighbor vertex
                vert = this->el_base[this->V[cur].elo + i];
                cand.reset(i);

                // only search thru neighbors with greater degrees
                if (this->V[vert].N < this->V[cur].N || vert == cur) continue;
                // (this amortizes the search cost because
                // vertices with really low degree have fewer neighbors
                // and vertices with really high degree will have been counted
                // by someone else)

                // clear the list of future candidates so that we can try with a
                // new set
                future_cand.clear();

                // assume vert is part of the clique
                res.set(i);

                // Check if vert has any common neighbors to cur
                // (apart from each other)
                for (k = 0; k < this->V[vert].N; k++) {
                    f = this->find_if_neighbors(
                        this->V[cur], el_base[this->V[vert].elo + k], ans);
                    // break if no more common neighbors
                    if (f == -1) break;

                    // if there is a common neighbor,
                    // add it to the list of candidates for the recursive call
                    else if (f == 1 && cand[ans]) {
                        future_cand.set(ans);
                        // cerr<<el_base[V[cur].elo+ans]<<"@"<<ans<<"\n";
                        f = 0;
                    }
                }
                // since you just checked the candidacy of vert,
                // you want to avoid checking it again
                future_cand.reset(i);

                // you also want to avoid checking cur itself in the recursive
                // call
                future_cand.reset(this->V[cur].spos);

                dfs_one_search(cur, future_cand, res);

                // we need to remove vert from clique consideration (undo
                // assumption) because the next (future) candidate may not have
                // it as part of the clique
                res.reset(i);
            }
        }
    }

    else if (mcs_potential == this->CUR_MAX_CLIQUE_SIZE) {
        // so this subtree has the "potential" to tie with the current maximum
        // but I don't want to waste the time searching for it
        // I'll just mark it for now, and check later
        this->V[cur].mcs = mcs_potential;
    }
}

void graph::dfs_one_clique(u32 cur) {
    graphBits res(this->V[cur].bits);
    graphBits cand = ~(this->V[cur].bits);
    dfs_one_search(cur, cand, res);
}

u32 graph::dfs_all_cliques(u32 start_vertex, double time_limit) {
    u32 i = start_vertex;
    TIME_LIMIT = time_limit;
    for (; i < vertices.size(); i++) {
        if ((clock() - duration) / CLOCKS_PER_SEC > TIME_LIMIT) {
            cerr << "DFS: Exceeded time limit of " << TIME_LIMIT
                 << " seconds\n";
            break;
        }
        if (V[i].N <= CUR_MAX_CLIQUE_SIZE || CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT)
            continue;
        dfs_one_clique(i);
    }
    // If we pause midway, I want to know where we stopped
    return i;
}
