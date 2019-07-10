#include <cm_base/include/core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

// A version of dfs_one_search that
// finds A maximum clique
// containing the vertex cur
void graph::dfs_other_search(u32 cur, graphBits& prev_cand,
                             graphBits& prev_res) {
    if (this->V[cur].mcs >= CLIQUE_LIMIT) return;
    u32 candidates_left = prev_cand.count();
    u32 mcs_potential = candidates_left + prev_res.count();

    if (mcs_potential >= this->V[cur].mcs) {
        if (candidates_left == 0) {
            this->V[cur].bits = prev_res;
            this->V[cur].mcs = mcs_potential;
            return;
        }

        else {
            graphBits cand(prev_cand);
            graphBits res(prev_res);
            graphBits future_cand(this->V[cur].N);

            u32 i, k;
            short f = 0;
            u32 vert, ans;
            for (i = 0; i < this->V[cur].N; i++) {
                if (cand.block_empty(i)) {
                    i += (31 - i % 32);
                    continue;
                }
                if (!cand[i]) continue;

                vert = this->el_base[this->V[cur].elo + i];
                cand.reset(i);

                if (this->V[vert].N < this->V[cur].N || vert == cur) continue;
                future_cand.clear();
                res.set(i);

                for (k = 0; k < this->V[vert].N; k++) {
                    f = binary_find(el_base + V[cur].elo, V[cur].N,
                                    el_base[V[vert].elo + k], ans);

                    if (f == -1)
                        break;
                    else if (f == 1 && cand[ans]) {
                        future_cand.set(ans);
                        f = 0;
                    }
                }
                future_cand.reset(i);
                future_cand.reset(V[cur].spos);
                dfs_other_search(cur, future_cand, res);
                res.reset(i);
            }
        }
    }
}

void graph::dfs_other_clique(u32 cur, u32 limit) {
    graphBits res(this->V[cur].bits);
    graphBits cand = ~(this->V[cur].bits);
    u32 temp = this->CLIQUE_LIMIT;
    this->CLIQUE_LIMIT = limit;
    this->V[cur].mcs = 0;
    dfs_other_search(cur, cand, res);
    this->CLIQUE_LIMIT = temp;
}

void graph::dfs_other_clique(u32 cur) {
    dfs_other_clique(cur, this->CUR_MAX_CLIQUE_SIZE);
}

std::vector<u32> graph::possible_others() {
    u32 i;
    std::vector<u32> ans;
    for (i = 0; i < this->n_vert; i++) {
        if (this->V[i].mcs == CUR_MAX_CLIQUE_SIZE) ans.push_back(i);
    }
    return ans;
}
