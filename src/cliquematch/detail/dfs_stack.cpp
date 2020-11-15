/* dfs_stack.cpp
 *
 * stack-based implementation of the clique search. Note that dfs_recurse.cpp
 * implements the SAME algorithm, but with recursion.  The difference is that
 * StackDFS uses data members to avoid some computations (it computes them
 * incrementally) and StackDFS checks the clique potential before making the
 * recursive call (i.e. pushing on to the stack). StackDFS does not have a time
 * limit.
 *
 * Two heap allocations are made: when process_graph is called,
 * StackDFS::to_remove and StackDFS::states reserve space equal to the upper
 * bound on the clique size. All other variables are on the stack. Heap memory
 * from the graph object is re-used in the load_memory/clear_memory calls,
 * which are explicitly pointed out; every load must have a matching clear or
 * there will be memory corruption.
 *
 * Compare with RecursionDFS when changing the implementation.
 */
#include <detail/dfs.h>

namespace cliquematch
{
namespace detail
{
    std::size_t StackDFS::process_graph(graph& G)
    {
        // the upper bound on clique size is the maximum depth on the stack
        this->states.reserve(G.CLIQUE_LIMIT);
        this->to_remove.reserve(G.CLIQUE_LIMIT);
        for (i = 0; i < G.n_vert; i++)
        {
            if (G.vertices[i].mcs <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
            process_vertex(G, i);
        }
        // Dummy return. i = G.n_vert, because the search cannot be interrupted
        return i;
    }

    void StackDFS::process_vertex(graph& G, std::size_t cur)
    {
        f = 0;
        request_size = (G.vertices[cur].N % BITS_PER_SIZE_T != 0) +
                       G.vertices[cur].N / BITS_PER_SIZE_T;
        // "memory" allocations for cand, res at root of subtree
        SearchState x(G.vertices[cur], G.load_memory(request_size),
                      G.load_memory(request_size));
        this->clique_potential = 1;

        // only search thru neighbors with greater degrees
        // (this amortizes the search cost because
        // vertices with really low degree have fewer neighbors
        // and vertices with really high degree will have been counted
        // by someone else)
        for (j = 0; j < G.vertices[cur].spos; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N <= G.vertices[cur].N) continue;
            x.cand.set(j);
            this->clique_potential++;
        }

        // vertices with a greater label haven't been the root of a search tree
        // yet, hence also consider those with equal degrees; the above loop
        // will ensure the search subtree is not re-traversed
        for (j = G.vertices[cur].spos + 1; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N < G.vertices[cur].N) continue;
            x.cand.set(j);
            this->clique_potential++;
        }

        if (this->clique_potential <= G.CUR_MAX_CLIQUE_SIZE)
        {
            G.clear_memory(2 * request_size);  // clear x.cand, x.res
            return;
        }

        // always use std::move when pushing on to stack
        states.push_back(std::move(x));
        clique_size = 1;

        while (!states.empty())
        {
            if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) break;
            // strong assumption:
            // the top of the stack always leads to a clique larger than the current max
            // (checking is done before pushing on to the stack)
            SearchState& cur_state = states.back();
            candidates_left = cur_state.cand.count();
            // clique_size == cur_state.res.count()
            //
            // because clique_size (and effectively clique_potential)
            // are changed every time the stack changes,
            // calling res.count() is unnecessary.

            for (j = cur_state.start_at; j < G.vertices[cur].N; j++)
            {
                if (!cur_state.cand[j]) continue;
                vert = G.edge_list[G.vertices[cur].elo + j];
                cur_state.cand.reset(j);
                cur_state.start_at = j + 1;
                candidates_left--;
                clique_potential = candidates_left + 1 + clique_size;

                // ensure only the vertices found in the below loop are removed later
                to_remove.clear();
                for (k = j + 1;
                     k < G.vertices[cur].N && clique_potential > G.CUR_MAX_CLIQUE_SIZE;
                     k++)
                {
                    if (!cur_state.cand[k]) continue;
                    f = G.find_if_neighbors(vert, G.edge_list[G.vertices[cur].elo + k],
                                            ans);
                    if (f != 1) to_remove.push_back(k);
                    f = 0;
                    clique_potential =
                        (candidates_left - to_remove.size()) + clique_size + 1;
                }

                // is the current maximum beatable?
                if (clique_potential > G.CUR_MAX_CLIQUE_SIZE)
                {
                    // no candidates left => clique cannot grow
                    // therefore clique_potential = clique size + 1; the edge (cur,
                    // vert) and this clique has beaten the existing maximum
                    if (candidates_left == 0)
                    {
                        // include vert as part of the clique and copy
                        cur_state.res.set(j);
                        G.vertices[cur].bits.copy_data(cur_state.res);
                        G.vertices[cur].mcs = clique_potential;
                        G.CUR_MAX_CLIQUE_SIZE = clique_potential;
                        G.CUR_MAX_CLIQUE_LOC = cur;

                        // search can now continue without vert
                        cur_state.res.reset(j);
                    }

                    else  // clique may still grow to beat the maximum
                    {
                        SearchState future_state(j);
                        // allocate memory for another SearchState on the stack.
                        // no need to allocate memory for res, refer_from passes the
                        // reference of the cur_state.res much like res is passed in
                        // RecursionDFS
                        future_state.refer_from(G.load_memory(request_size),
                                                cur_state.cand, cur_state.res);

                        // remove invalid members from the candidate set
                        for (auto k : to_remove) future_state.cand.reset(k);

                        // clique_potential check has happened before pushing on to the
                        // stack; strong assumption is therefore valid
                        states.push_back(std::move(future_state));

                        // clique_size has increased by 1 due to vert
                        clique_size++;
                        // the top of the stack has changed,
                        // prevent any further operations on cur_state
                        break;
                    }
                }
                // clique_potential <= CUR_MAX_CLIQUE_SIZE, so
                // this subtree cannot beat the maximum.
                // consider the next value for vert and try again.
            }

            // all verts with id > cur_state.id have been checked
            if (j == G.vertices[cur].N)
            {
                // all potential cliques that can contain (cur, v) have been searched
                // where v is at position cur_state.id in list of cur's neighbors
                states.pop_back();
                G.clear_memory(request_size);

                // remove v from clique consideration because the future
                // candidates will not have it as part of the clique
                cur_state.res.reset(cur_state.id);
                clique_size--;
            }
        }

        if (!states.empty())  // the search was terminated due to CLIQUE_LIMIT
        {
            // release memory of all candidates on the stack
            G.clear_memory(request_size * states.size());
            states.clear();
        }

        G.clear_memory(request_size);
        // release memory of x.res i.e. the space for
        // the clique allocated at the base of the stack
    }
}  // namespace detail
}  // namespace cliquematch
