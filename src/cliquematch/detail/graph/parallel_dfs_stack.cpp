/* parallel_dfs_stack.cpp
 *
 * stack-based implementation of the clique search, but running parallel with
 * std::thread.
 *
 * Two heap allocations are made: when process_graph is called,
 * StackDFS::to_remove and StackDFS::states reserve space equal to the upper
 * bound on the clique size. All other variables are on the stack. Heap memory
 * from the graph object is re-used in the load_memory/clear_memory calls,
 * which are explicitly pointed out; every load must have a matching clear or
 * there will be memory corruption.
 *
 */
#include <detail/graph/dfs.h>

#ifndef NUM_THREADS
#define NUM_THREADS 2
#endif

namespace cliquematch
{
namespace detail
{
    u64 ParallelStackDFS::process_graph(graph &G)
    {
        current_vertex = 0;
        std::thread *workers = new std::thread[NUM_THREADS];
        for (int tt = 0; tt < NUM_THREADS; ++tt) {
            workers[tt] = std::thread(&ParallelStackDFS::start_context, this, std::ref(G));
        }
        for (int tt = 0; tt < NUM_THREADS; ++tt) {
            workers[tt].join();
        }
        delete[] workers;
        return current_vertex;
    }

    void ParallelStackDFS::start_context(graph &G)
    {
        DFSContext ctx;
        ctx.states.reserve(G.CLIQUE_LIMIT);
        ctx.to_remove.reserve(G.CLIQUE_LIMIT);
        while (1)
        {
            this->vert_mutex.lock();
            ctx.cur = this->current_vertex++;
            this->vert_mutex.unlock();
            if (ctx.cur >= G.n_vert) { break; }
            if (G.vertices[ctx.cur].mcs <= G.CUR_MAX_CLIQUE_SIZE ||
                G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT)
                continue;
            process_vertex(G, ctx);
        }
    }

    void ParallelStackDFS::process_vertex(graph &G, DFSContext &ctx)
    {
        u64 j, k, vert, start, ans;
        //
        const u64 cur = ctx.cur;
        const u64 request_size =
            (G.vertices[cur].N % BITS_PER_U64 != 0) + G.vertices[cur].N / BITS_PER_U64;
        u64 *res = new u64[request_size];
        SearchState x(G.vertices[cur], res, new u64[request_size]);
        x.res.clear();
        x.cand.clear();
        x.res.set(G.vertices[cur].spos);
        ctx.clique_potential = 1;

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
            ctx.clique_potential++;
        }

        // vertices with a greater label haven't been the root of a search tree
        // yet, hence also consider those with equal degrees; the above loop
        // will ensure the search subtree is not re-traversed
        for (j = G.vertices[cur].spos + 1; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N < G.vertices[cur].N) continue;
            x.cand.set(j);
            ctx.clique_potential++;
        }

        if (ctx.clique_potential <= G.CUR_MAX_CLIQUE_SIZE)
        {
            delete[] x.res.data;
            delete[] x.cand.data;
            return;
        }

        // always use std::move when pushing on to stack
        ctx.states.push_back(std::move(x));
        ctx.clique_size = 1;

        start = 0;
        while (!ctx.states.empty())
        {
            if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) break;
            // strong assumption:
            // the top of the stack always leads to a clique larger than the current max
            // (checking is done before pushing on to the stack)
            SearchState &cur_state = ctx.states.back();
            ctx.candidates_left = cur_state.cand.count();
            // clique_size == cur_state.res.count()
            //
            // because clique_size (and effectively clique_potential)
            // are changed every time the stack changes,
            // calling res.count() is unnecessary.

            // in case cur_state.start_at was cleared,
            // move to the next valid position now,
            // so when we return to cur_state it is proper
            cur_state.start_at = cur_state.cand.next(cur_state.start_at);

            for (j = cur_state.start_at; j < G.vertices[cur].N; j = cur_state.start_at)
            {
                cur_state.cand.reset(j);
                cur_state.start_at = cur_state.cand.next(j + 1);
                ctx.candidates_left--;
                ctx.clique_potential = ctx.candidates_left + 1 + ctx.clique_size;

                // ensure only the vertices found in the below loop are removed later
                ctx.to_remove.clear();

                vert = G.edge_list[G.vertices[cur].elo + j];
                start = G.vertices[vert].spos;

                for (k = cur_state.start_at;
                     k < G.vertices[cur].N &&
                     ctx.clique_potential > G.CUR_MAX_CLIQUE_SIZE;
                     k = cur_state.cand.next(k + 1))
                {
                    if (binary_find(&(G.edge_list[G.vertices[vert].elo + start]),
                                    G.vertices[vert].N - start,
                                    G.edge_list[G.vertices[cur].elo + k], ans) != FOUND)
                        ctx.to_remove.push_back(k);

                    start += ans;
                    ctx.clique_potential =
                        (ctx.candidates_left - ctx.to_remove.size()) + ctx.clique_size + 1;
                }

                // is the current maximum beatable?
                if (ctx.clique_potential > G.CUR_MAX_CLIQUE_SIZE)
                {
                    // no candidates left => clique cannot grow

                    // clique_potential = clique size + 1 ie the edge (cur, vert)
                    // this clique has beaten the existing maximum
                    if (ctx.candidates_left == 0)
                    {
                        // include vert as part of the clique and copy
                        cur_state.res.set(j);
                        clq_mutex.lock();
                        if (ctx.clique_potential > G.CUR_MAX_CLIQUE_SIZE)
                        {
                            // ok now we're surely the max, so update
                            G.vertices[cur].bits.copy_data(cur_state.res);
                            G.vertices[cur].mcs = ctx.clique_potential;
                            G.CUR_MAX_CLIQUE_SIZE = ctx.clique_potential;
                            G.CUR_MAX_CLIQUE_LOCATION = cur;
                        }
                        // search can now continue without vert
                        clq_mutex.unlock();
                        cur_state.res.reset(j);
                    }

                    else  // clique may still grow to beat the maximum
                    {
                        SearchState future_state(j);
                        // allocate memory for another SearchState on the stack.
                        // no need to allocate memory for res, refer_from passes the
                        // reference of the cur_state.res much like res is passed in
                        // RecursionDFS
                        future_state.refer_from(new u64[request_size],
                                                cur_state.cand, cur_state.res);

                        // remove invalid members from the candidate set
                        for (auto kr : ctx.to_remove) future_state.cand.reset(kr);

                        // clique_potential check has happened before pushing on to the
                        // stack; strong assumption is therefore valid
                        ctx.states.push_back(std::move(future_state));

                        // clique_size has increased by 1 due to vert
                        ctx.clique_size++;
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
                delete[] cur_state.cand.data;

                // remove v from clique consideration because the future
                // candidates will not have it as part of the clique
                cur_state.res.reset(cur_state.id);
                ctx.clique_size--;

                ctx.states.pop_back();
            }
        }

        if (!ctx.states.empty())  // the search was terminated due to CLIQUE_LIMIT
        {
            // release memory of all candidates on the stack
            for (SearchState &st : ctx.states) { delete[] st.cand.data; }
            ctx.states.clear();
        }

        delete[] res;
        // release memory of x.res i.e. the space for
        // the clique allocated at the base of the stack
    }
}  // namespace detail
}  // namespace cliquematch
