/* dfs_recurse.cpp
 *
 * recursion-based implementation of the clique search. Note that dfs_stack.cpp
 * implements the SAME algorithm, but with an explicit stack to track the
 * recursion and some rearrangements for speedup.
 *
 * All variables are on the stack. No heap allocations are made. Heap memory
 * from the graph object is re-used in the load_memory/clear_memory calls,
 * which are explicitly pointed out; every load must have a matching clear or
 * there will be memory corruption.
 */

#include <detail/graph/dfs.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    u64 RecursionDFS::process_graph(graph& G)
    {
        u64 i = start_vertex;
        process_vertex(G, G.CUR_MAX_CLIQUE_LOCATION);
        for (; i < G.n_vert; i++)
        {
            if (G.vertices[i].mcs <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
            if (G.elapsed_time() > this->TIME_LIMIT) break;
            process_vertex(G, i);
        }
        // If paused midway due to time limits, I want to know where we stopped
        return i;
    }

    void RecursionDFS::process_vertex(graph& G, u64 cur)
    {
        request_size =
            (G.vertices[cur].N % BITS_PER_U64 != 0) + G.vertices[cur].N / BITS_PER_U64;

        // "memory" allocations for cand, res at root of subtree
        graphBits res(G.load_memory(request_size), G.vertices[cur].N, false);
        graphBits cand(G.load_memory(request_size), G.vertices[cur].N, false);
        res.set(G.vertices[cur].spos);
        u64 j, vert, clique_potential = 1;

        // only search thru neighbors with greater degrees
        // (this amortizes the search cost because
        // vertices with really low degree have fewer neighbors
        // and vertices with really high degree will have been counted
        // by someone else)
        for (j = 0; j < G.vertices[cur].spos; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N <= G.vertices[cur].N) continue;
            cand.set(j);
            clique_potential++;
        }

        // vertices with a greater label haven't been the root of a search tree
        // yet, hence also consider those with equal degrees; the above loop
        // will ensure the search subtree is not re-traversed
        for (j = G.vertices[cur].spos + 1; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N < G.vertices[cur].N) continue;
            cand.set(j);
            clique_potential++;
        }

        if (clique_potential > G.CUR_MAX_CLIQUE_SIZE) search_vertex(G, cur, cand, res);
        // search complete
        G.clear_memory(2 * request_size);  // releasing memory of cand, res
    }

    void RecursionDFS::search_vertex(graph& G, u64 cur, const graphBits& prev_cand,
                                     graphBits& res)
    {
        // weak assumption: cand, res may lead to a clique larger than the current max
        u64 candidates_left = prev_cand.count();
        u64 clique_potential = candidates_left + res.count();

        if (G.elapsed_time() > this->TIME_LIMIT)
            return;  // out of time, stop searching the graph immediately
        if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
            return;  // reached required upper bound, no need to search
        if (clique_potential <= G.CUR_MAX_CLIQUE_SIZE)
            return;  // expanding this subtree will not beat the maximum

        // no candidates left => clique cannot grow
        // therefore clique_potential is same as clique size
        // and this clique has beaten the existing maximum
        if (candidates_left == 0)
        {
            G.vertices[cur].bits.copy_data(res);
            G.vertices[cur].mcs = clique_potential;
            G.CUR_MAX_CLIQUE_SIZE = clique_potential;
            G.CUR_MAX_CLIQUE_LOCATION = cur;
            return;
        }

        // This clique may beat the existing maximum
        // There exist candidates for possible clique expansion,
        // so go through them recursively

        graphBits cand, future_cand;
        // "memory" allocations for cand and future_cand, remember to release
        cand.copy_from(prev_cand, G.load_memory(request_size));
        future_cand.copy_from(prev_cand, G.load_memory(request_size));

        u64 j, k, vert, start, ans = 0;
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            // keep going until a candidate exists
            if (!cand[j]) continue;

            // offset thru the edge list to get the neighbor vertex
            vert = G.edge_list[G.vertices[cur].elo + j];

            // assume vert is part of the clique
            res.set(j);
            cand.reset(j);

            // copy the list of candidates to pass to the recursive call
            future_cand.copy_data(cand);
            start = G.vertices[vert].spos + 1;

            // Check if the remaining candidates in cur are neighbors to vert
            for (k = j + 1; k < G.vertices[cur].N; k++)
            {
                if (!future_cand[k]) continue;
                if (binary_find(&(G.edge_list[G.vertices[vert].elo + start]),
                                G.vertices[vert].N - start,
                                G.edge_list[G.vertices[cur].elo + k], ans) != FOUND)
                {
                    future_cand.reset(k);
                }
                start += ans;
            }

            // search the subtree with the new set of candidates
            search_vertex(G, cur, future_cand, res);
            // all potential cliques that can contain (cur, vert) have been searched

            // remove vert from clique consideration because the future
            // candidates will not have it as part of the clique
            res.reset(j);
        }
        G.clear_memory(2 * request_size);  // releasing memory of cand, future_cand that
                                           // were allocated at this subtree
    }
}  // namespace detail
}  // namespace cliquematch
