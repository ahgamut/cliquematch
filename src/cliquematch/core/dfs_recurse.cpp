#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

void graph::dfs_one_search(size_t cur, const graphBits& prev_cand,
			   const graphBits& prev_res) {
    if (CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT) return;
    if ((clock() - duration) / CLOCKS_PER_SEC > TIME_LIMIT) return;

    size_t candidates_left = prev_cand.count();
    size_t mcs_potential = candidates_left + prev_res.count();

    if (mcs_potential > this->CUR_MAX_CLIQUE_SIZE) {
	// no candidates left => clique cannot grow
	// therefore mcs_potential is same as mcs
	if (candidates_left == 0) {
	    this->vertices[cur].bits = prev_res;
	    this->CUR_MAX_CLIQUE_SIZE = mcs_potential;
	    this->vertices[cur].mcs = mcs_potential;
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
	    graphBits future_cand(this->vertices[cur].N);

	    size_t i, k;
	    short f = 0;
	    size_t vert, ans;
	    for (i = 0; i < this->vertices[cur].N; i++) {
		// keep going until a candidate exists
		if (cand.block_empty(i)) {
		    i += (31 - i % 32);
		    continue;
		}
		if (!cand[i]) continue;

		// offset thru the edge list to get the neighbor vertex
		vert = this->edge_list[this->vertices[cur].elo + i];
		cand.reset(i);

		// only search thru neighbors with greater degrees
		if (this->vertices[vert].N < this->vertices[cur].N ||
		    vert == cur)
		    continue;
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
		for (k = 0; k < this->vertices[vert].N; k++) {
		    f = this->find_if_neighbors(
			this->vertices[cur],
			this->edge_list[this->vertices[vert].elo + k], ans);
		    // break if no more common neighbors
		    if (f == -1 && ans >= this->vertices[cur].N) break;

		    // if there is a common neighbor,
		    // add it to the list of candidates for the recursive call
		    else if (f == 1 && cand[ans]) {
			future_cand.set(ans);
			// cerr<<this->edge_list[this->vertices[cur].elo+ans]<<"@"<<ans<<"\n";
			f = 0;
		    }
		}
		// since you just checked the candidacy of vert,
		// you want to avoid checking it again
		future_cand.reset(i);

		// you also want to avoid checking cur itself in the recursive
		// call
		future_cand.reset(this->vertices[cur].spos);

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
	this->vertices[cur].mcs = mcs_potential;
    }
}

#ifndef STACK_DFS
#pragma message("Using recursion for DFS")
void graph::dfs_one_clique(size_t cur) {
    graphBits res(this->vertices[cur].bits);
    graphBits cand = ~(this->vertices[cur].bits);
    dfs_one_search(cur, cand, res);
}
#endif

