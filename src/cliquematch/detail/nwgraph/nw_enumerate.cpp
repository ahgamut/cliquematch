#include <detail/nwgraph/nwclique.h>

namespace cliquematch
{
namespace detail
{
    void NWCliqueEnumerator::process_vertex(nwgraph& G)
    {
        // continue until vertex this->cur can possibly build a clique of
        // REQUIRED_WEIGHT
        for (cur++; cur < G.vertices.size(); cur++)
        {
            if (G.vertices[cur].mcs < this->REQUIRED_WEIGHT) continue;
            if (load_vertex(G)) break;
        }
        // there are no more vertices that can possibly build a clique of
        // REQUIRED_WEIGHT
    }

    // return true if the vertex this->cur can possibly build a clique of
    // REQUIRED_WEIGHT
    bool NWCliqueEnumerator::load_vertex(nwgraph& G)
    {
        request_size =
            (G.vertices[cur].N % BITS_PER_U64 != 0) + G.vertices[cur].N / BITS_PER_U64;

        // "memory" allocations for cand, res at root of subtree
        NWSearchState x(G.vertices[cur], G.load_memory(request_size),
                        G.load_memory(request_size));
        this->clique_potential = 0;

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
            weights[j] = G.vertices[vert].weight;
            this->clique_potential += weights[j];
        }

        // vertices with a greater label haven't been the root of a search tree
        // yet, hence also consider those with equal degrees; the above loop
        // will ensure the search subtree is not re-traversed
        for (j = G.vertices[cur].spos + 1; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].N < G.vertices[cur].N) continue;
            x.cand.set(j);
            weights[j] = G.vertices[vert].weight;
            this->clique_potential += weights[j];
        }

        if (this->clique_potential + G.vertices[cur].weight < this->REQUIRED_WEIGHT)
        {
            G.clear_memory(2 * request_size);  // release x.cand, x.res
            return false;
        }

        x.potential = this->clique_potential;

        // always use std::move when pushing on to stack
        states.push_back(std::move(x));
        clique_weight = G.vertices[cur].weight;

        return true;
    }

    // return id of the vertex where a clique of REQUIRED_WEIGHT was found
    // CALLER checks if returned value > G.n_vert => search is complete
    u64 NWCliqueEnumerator::process_graph(nwgraph& G)
    {
        // set maximum depth on the stack
        this->states.reserve(G.max_depth);
        this->to_remove.reserve(G.max_depth);
        this->weights.reserve(G.max_neighbors);

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
            NWSearchState& cur_state = states.back();
            candidates_left = cur_state.cand.count();
            // clique_size == cur_state.res.count()
            //
            // because clique_size (and effectively clique_potential)
            // are changed every time the stack changes,
            // calling res.count() is unnecessary.

            for (j = cur_state.start_at; j < G.vertices[cur].N; j++)
            {
                if (!cur_state.cand[j]) continue;
                cur_state.cand.reset(j);
                cur_state.start_at = j + 1;
                cur_state.potential -= this->weights[j];
                candidates_left -= 1;

                cand_potential = cur_state.potential;
                clique_potential = cand_potential + clique_weight + this->weights[j];

                vert = G.edge_list[G.vertices[cur].elo + j];

                // ensure only the vertices found in the below loop are removed later
                to_remove.clear();
                for (k = j + 1;
                     k < G.vertices[cur].N && clique_potential > this->REQUIRED_WEIGHT;
                     k++)
                {
                    if (!cur_state.cand[k]) continue;
                    if (binary_find(&(G.edge_list[G.vertices[vert].elo +
                                                  G.vertices[vert].spos]),
                                    G.vertices[vert].N - G.vertices[vert].spos,
                                    G.edge_list[G.vertices[cur].elo + k], ans) != FOUND)
                    {
                        to_remove.push_back(k);
                        cand_potential -= this->weights[k];
                        candidates_left -= 1;
                    }
                    clique_potential =
                        cand_potential + clique_weight + this->weights[j];
                }

                // is it possible to produce a clique of REQUIRED_WEIGHT?
                if (clique_potential >= this->REQUIRED_WEIGHT)
                {
                    // now candidates_left may be nonzero, but only the clique size is
                    // necessary. Add 1 so as to count vert as part of the clique
                    if (clique_weight + weights[j] >= this->REQUIRED_WEIGHT)
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

                    else  // clique may still grow to REQUIRED_WEIGHT
                    {
                        NWSearchState future_state(j, cand_potential);

                        // allocate memory for another NWSearchState on the stack.
                        // no need to allocate memory for res, refer_from passes the
                        // reference of the cur_state.res much like res is passed in
                        // RecursionDFS
                        future_state.refer_from(G.load_memory(request_size),
                                                cur_state.cand, cur_state.res);

                        // clique_size has increased due to vert
                        clique_weight += this->weights[j];

                        // remove invalid members from the candidate set
                        for (auto k : to_remove) future_state.cand.reset(k);

                        // clique_potential check has happened before pushing on to the
                        // stack; strong assumption is therefore valid
                        states.push_back(std::move(future_state));

                        // the top of the stack has changed,
                        // prevent any further operations on cur_state
                        break;
                    }
                }
                // clique_potential <= REQUIRED_WEIGHT, so
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
                clique_weight -= weights[cur_state.id];
            }
        }

        // now cur = G.n_vert, there are no more cliques to be found
        // the search stack is empty, and
        // all the necessary G.clear_memory() calls have been made
        return cur;
    }

}  // namespace detail
}  // namespace cliquematch
