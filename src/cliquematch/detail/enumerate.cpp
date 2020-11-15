/* enumerate.cpp
 *
 * stack-based implementation of the clique search that returns every time a
 * clique of REQUIRED_SIZE is found.  It returns the vertex that was the root
 * of the subtree that found the clique. The CALLER can call G.get_max_clique
 * to get the clique. The search can be resumed by calling process_graph again.
 * The search is complete if the returned vertex has an ID > the number of
 * vertices in the provided graph.
 *
 * This process of repeated calls is congruent with __next__() calls for Python
 * iterators. (Easy to write a pybind11 wrapper: just call stop_iteration()
 * when the search is done).
 *
 * The structure follows that of StackDFS, with notable changes as to how the
 * functions terminate/return. The stack is saved when the function returns, so
 * the search can be resumed. An interesting feature is that the cliques
 * returned may or may not be maximal: if a graph G has a maximum clique S of
 * size 25, then CliqueEnumerator will enumerate at least 25 cliques of size 24
 * (the sub-cliques of S).
 */
#include <detail/dfs.h>

namespace cliquematch
{
namespace detail
{
    void CliqueEnumerator::process_vertex(graph& G)
    {
        // continue until vertex this->cur can possibly build a clique of REQUIRED_SIZE
        for (cur++; cur < G.vertices.size(); cur++)
        {
            if (G.vertices[cur].mcs < this->REQUIRED_SIZE) continue;
            if (load_vertex(G)) break;
        }
        // there are no more vertices that can possibly build a clique of REQUIRED_SIZE
    }

    // return true if the vertex this->cur can possibly build a clique of REQUIRED_SIZE
    bool CliqueEnumerator::load_vertex(graph& G)
    {
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

        if (this->clique_potential < this->REQUIRED_SIZE)
        {
            G.clear_memory(2 * request_size);  // release x.cand, x.res
            return false;
        }

        // always use std::move when pushing on to stack
        states.push_back(std::move(x));
        clique_size = 1;

        return true;
    }

    // return id of the vertex where a clique of REQUIRED_SIZE was found
    // CALLER checks if returned value > G.n_vert => search is complete
    std::size_t CliqueEnumerator::process_graph(graph& G)
    {
        if (this->REQUIRED_SIZE == 0)
        {  // 0 cliques of size 0, dummy case
            cur = G.n_vert;
            return cur;
        }
        else if (this->REQUIRED_SIZE == 1 && cur < G.n_vert)
        {  // every vertex is a clique of size 1, dummy casse
            G.vertices[cur].bits.clear();
            G.vertices[cur].bits.set(G.vertices[cur].spos);
            return cur++;
        }

        // the upper bound on clique size is the maximum depth on the stack
        this->states.reserve(G.CLIQUE_LIMIT);
        this->to_remove.reserve(G.CLIQUE_LIMIT);
        f = 0;

        while (cur < G.n_vert)  // go through all vertices of the graph
        {
            if (states.empty())
            {
                // when constructed, request_size = 0, so nothing happens
                G.clear_memory(request_size);
                // later, this releases memory of x.res i.e. the space for
                // the clique allocated at the base of the stack
                this->process_vertex(G);
                // now request_size has changed according to the new vertex
                // (i.e. a new SearchState has been pushed onto base of the stack)
                // OR there are no more vertices left
                continue;  // check if cur < G.n_vert
            }

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
                     k < G.vertices[cur].N && clique_potential >= this->REQUIRED_SIZE;
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

                // is it possible to produce a clique of REQUIRED_SIZE?
                if (clique_potential >= this->REQUIRED_SIZE)
                {
                    // now candidates_left may be nonzero, but only the clique size is
                    // necessary. Add 1 so as to count vert as part of the clique
                    if (clique_size + 1 == this->REQUIRED_SIZE)
                    {
                        cur_state.res.set(j);
                        G.vertices[cur].bits.copy_data(cur_state.res);

                        // search can now continue without vert
                        cur_state.res.reset(j);

                        // now the caller can call G.get_max_clique(cur)
                        return cur;
                        // note that the top of the stack has not been changed.
                        // when process_graph is called again, it will resume searching
                        // at this subtree.
                    }

                    else  // clique may still grow to REQUIRED_SIZE
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

                        // clique_potential has been checked before pushing on to the
                        // stack; strong assumption is therefore valid
                        states.push_back(std::move(future_state));

                        // clique_size has increased by 1 due to vert
                        clique_size++;
                        // the top of the stack has changed,
                        // prevent any further operations on cur_state
                        break;
                    }
                }
                // clique_potential <= REQUIRED_SIZE, so
                // this subtree cannot produce any required clique.
                // consider the next value for vert and try again.
            }

            // all verts with id > cur_state.id have been checked
            if (j >= G.vertices[cur].N)
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

        // now cur = G.n_vert, there are no more cliques to be found
        // the search stack is empty, and
        // all the necessary G.clear_memory() calls have been made
        return cur;
    }

}  // namespace detail
}  // namespace cliquematch
