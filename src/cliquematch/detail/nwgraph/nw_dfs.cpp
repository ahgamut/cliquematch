#include <detail/nwgraph/nwclique.h>

namespace cliquematch
{
namespace detail
{
    u64 NWStackDFS::process_graph(nwgraph& G)
    {
        // the upper bound on clique size is the maximum depth on the stack
        this->states.reserve(G.max_depth);
        this->to_remove.reserve(G.max_depth);
        this->weights.reserve(G.max_neighbors);
        this->process_vertex(G, G.CUR_MAX_CLIQUE_LOCATION);
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

    void NWStackDFS::process_vertex(nwgraph& G, u64 cur)
    {
        double cand_potential = 0;
        request_size =
            (G.vertices[cur].N % BITS_PER_U64 != 0) + G.vertices[cur].N / BITS_PER_U64;
        // "memory" allocations for cand, res at root of subtree
        NWSearchState x(G.vertices[cur], G.load_memory(request_size),
                        G.load_memory(request_size));
        this->clique_potential = 0;

        this->weights.clear();
        // only search thru neighbors with greater degrees
        // (this amortizes the search cost because
        // vertices with really low degree have fewer neighbors
        // and vertices with really high degree will have been counted
        // by someone else)
        for (j = 0; j < G.vertices[cur].spos; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].degree <= G.vertices[cur].degree) continue;
            x.cand.set(j);
            this->clique_potential += G.vertices[vert].weight;
            this->weights[j] = G.vertices[vert].weight;
        }

        // vertices with a greater label haven't been the root of a search tree
        // yet, hence also consider those with equal degrees; the above loop
        // will ensure the search subtree is not re-traversed
        for (j = G.vertices[cur].spos + 1; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (G.vertices[vert].degree < G.vertices[cur].degree) continue;
            x.cand.set(j);
            this->clique_potential += G.vertices[vert].weight;
            this->weights[j] = G.vertices[vert].weight;
        }

        if (this->clique_potential + G.vertices[cur].weight <= G.CUR_MAX_CLIQUE_SIZE)
        {
            G.clear_memory(2 * request_size);  // clear x.cand, x.res
            return;
        }

        x.potential = this->clique_potential;

        // always use std::move when pushing on to stack
        states.push_back(std::move(x));
        clique_weight = G.vertices[cur].weight;

        while (!states.empty())
        {
            if (G.CUR_MAX_CLIQUE_SIZE >= G.CLIQUE_LIMIT) break;
            // strong assumption:
            // the top of the stack always leads to a clique larger than the current max
            // (checking is done before pushing on to the stack)
            NWSearchState& cur_state = states.back();
            candidates_left = cur_state.cand.count();

            // in case cur_state.start_at was cleared,
            // move to the next valid position now,
            // so when we return to cur_state it is proper
            cur_state.start_at = cur_state.cand.next(cur_state.start_at);

            for (j = cur_state.start_at; j < G.vertices[cur].N; j = cur_state.start_at)
            {
                cur_state.cand.reset(j);
                cur_state.start_at = cur_state.cand.next(j + 1);
                cur_state.potential -= this->weights[j];
                candidates_left = cur_state.cand.count();

                cand_potential = 0;
                vert = G.edge_list[G.vertices[cur].elo + j];

                // ensure only the vertices found in the below loop are removed later
                to_remove.clear();
                for (k = cur_state.start_at;
                     k < G.vertices[cur].N && candidates_left != 0;
                     k = cur_state.cand.next(k + 1))
                {
                    if (binary_find(&(G.edge_list[G.vertices[vert].elo +
                                                  G.vertices[vert].spos]),
                                    G.vertices[vert].N - G.vertices[vert].spos,
                                    G.edge_list[G.vertices[cur].elo + k], ans) != FOUND)
                    {
                        to_remove.push_back(k);
                        candidates_left -= 1;
                    }
                    else
                        cand_potential += this->weights[k];
                }
                clique_potential = cand_potential + clique_weight + this->weights[j];


                // is the current maximum beatable?
                if (clique_potential > G.CUR_MAX_CLIQUE_SIZE)
                {
                    // no candidates left => clique cannot grow
                    // therefore clique_potential = clique size + vert.weight
                    // this clique has beaten the existing maximum
                    if (candidates_left == 0)
                    {
                        // include vert as part of the clique and copy
                        cur_state.res.set(j);
                        G.vertices[cur].bits.copy_data(cur_state.res);
                        G.vertices[cur].mcs = clique_potential;
                        G.CUR_MAX_CLIQUE_SIZE = clique_potential;
                        G.CUR_MAX_CLIQUE_LOCATION = cur;

                        // search can now continue without vert
                        cur_state.res.reset(j);
                    }

                    else  // clique may still grow to beat the maximum
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
                clique_weight -= this->weights[cur_state.id];
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
