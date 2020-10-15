#include <detail/dfs.h>

namespace cliquematch
{
namespace detail
{
    void CliqueEnumerator::process_vertex(graph& G)
    {
        for (cur++; cur < G.vertices.size(); cur++)
        {
            if (G.vertices[cur].N < this->REQUIRED_SIZE) continue;
            if (load_vertex(G)) break;
        }
    }

    bool CliqueEnumerator::load_vertex(graph& G)
    {
        SearchState x(G.vertices[cur], G.recycle_memory(G.vertices[cur].N),
                      G.recycle_memory(G.vertices[cur].N));
        this->clique_potential = 1;
        for (j = 0; j < G.vertices[cur].N; j++)
        {
            vert = G.edge_list[G.vertices[cur].elo + j];
            if (vert == cur || G.vertices[vert].N < G.vertices[cur].N ||
                (G.vertices[vert].N == G.vertices[cur].N && vert < cur))
                continue;
            x.cand.set(j);
            this->clique_potential++;
        }
        if (this->clique_potential < this->REQUIRED_SIZE) return false;
        states.push_back(std::move(x));
        clique_size = 1;
        return true;
    }

    std::size_t CliqueEnumerator::process_graph(graph& G)
    {
        f = 0;
        if (this->REQUIRED_SIZE == 0)
        {
            cur = G.n_vert;
            return cur;
        }
        if (this->REQUIRED_SIZE == 1 && cur < G.n_vert)
        {
            G.vertices[cur].bits.clear();
            G.vertices[cur].bits.set(G.vertices[cur].spos);
            return cur++;
        }
        states.reserve(G.max_degree);

        while (cur < G.n_vert)
        {
            if (states.empty())
            {
                G.check_memory();
                this->process_vertex(G);
                continue;
            }
            SearchState& cur_state = states.back();
            candidates_left = cur_state.cand.count();
            clique_potential = candidates_left + clique_size;

            for (j = cur_state.start_at; j < G.vertices[cur].N; j++)
            {
                if (!cur_state.cand[j]) continue;
                vert = G.edge_list[G.vertices[cur].elo + j];
                cur_state.cand.reset(j);
                cur_state.start_at = j + 1;

                to_remove.clear();
                for (k = j + 1;
                     k < G.vertices[cur].N && clique_potential >= this->REQUIRED_SIZE;
                     k++)
                {
                    if (!cur_state.cand[k]) continue;
                    f = G.find_if_neighbors(vert, G.edge_list[G.vertices[cur].elo + k],
                                            ans);
                    if (f != 1) to_remove.emplace_back(k);
                    f = 0;
                    clique_potential =
                        (candidates_left - to_remove.size()) + clique_size;
                }

                if (clique_potential >= this->REQUIRED_SIZE)
                {
                    // adding 1 so as to count vert as part of the clique
                    if (clique_size + 1 == this->REQUIRED_SIZE)
                    {
                        cur_state.res.set(j);
                        G.vertices[cur].bits.copy_data(cur_state.res);
                        cur_state.res.reset(j);
                        return cur;
                    }

                    else
                    {
                        SearchState future_state;
                        future_state.refer_from(G.recycle_memory(G.vertices[cur].N),
                                                cur_state.cand, cur_state.res, j);
                        for (auto k : to_remove) future_state.cand.reset(k);
                        states.push_back(std::move(future_state));
                        clique_size++;
                        break;
                    }
                }
            }
            if (j >= G.vertices[cur].N)
            {
                cur_state.res.reset(cur_state.id);
                states.pop_back();
                clique_size--;
            }
        }

        return cur;
    }

}  // namespace detail
}  // namespace cliquematch
