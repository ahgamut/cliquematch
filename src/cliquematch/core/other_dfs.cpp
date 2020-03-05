#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

// A version of dfs_one_search that
// finds A maximum clique
// containing the vertex cur
void graph::dfs_other_search(u32 cur, const graphBits& prev_cand,
			     const graphBits& prev_res) {
    if (this->vertices[cur].mcs >= CLIQUE_LIMIT) return;
    u32 candidates_left = prev_cand.count();
    u32 mcs_potential = candidates_left + prev_res.count();

    if (mcs_potential >= this->vertices[cur].mcs) {
	if (candidates_left == 0) {
	    this->vertices[cur].bits = prev_res;
	    this->vertices[cur].mcs = mcs_potential;
	    return;
	}

	else {
	    graphBits cand(prev_cand);
	    graphBits res(prev_res);
	    graphBits future_cand(this->vertices[cur].N);

	    u32 i, k;
	    short f = 0;
	    u32 vert, ans;
	    for (i = 0; i < this->vertices[cur].N; i++) {
		if (cand.block_empty(i)) {
		    i += (31 - i % 32);
		    continue;
		}
		if (!cand[i]) continue;

		vert = this->edge_list[this->vertices[cur].elo + i];
		cand.reset(i);

		if (this->vertices[vert].N < this->vertices[cur].N ||
		    vert == cur)
		    continue;
		future_cand.clear();
		res.set(i);

		for (k = 0; k < this->vertices[vert].N; k++) {
		    f = this->find_if_neighbors(
			this->vertices[cur],
			this->edge_list[this->vertices[vert].elo + k], ans);
		    if (f == -1)
			break;
		    else if (f == 1 && cand[ans]) {
			future_cand.set(ans);
			f = 0;
		    }
		}
		future_cand.reset(i);
		future_cand.reset(this->vertices[cur].spos);
		dfs_other_search(cur, future_cand, res);
		res.reset(i);
	    }
	}
    }
}

void graph::dfs_other_clique(u32 cur, u32 limit) {
    graphBits res(this->vertices[cur].bits);
    graphBits cand = ~(this->vertices[cur].bits);
    u32 temp = this->CLIQUE_LIMIT;
    this->CLIQUE_LIMIT = limit;
    this->vertices[cur].mcs = 0;
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
	if (this->vertices[i].mcs == CUR_MAX_CLIQUE_SIZE) ans.push_back(i);
    }
    return ans;
}
