#include <detail/nwgraph/nwgraph.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    /* I use u64 everywhere, so I know I am definitely getting a 64bit value
     * to work with during any calculation.
     */

    nwgraph::nwgraph()
    {
        n_vert = 0;
        el_size = 0;
        eb_size = 0;
        search_start = 0;
        search_end = 0;
        max_neighbors = 0;
        max_depth = 0;

        CUR_MAX_CLIQUE_LOCATION = 0;
        CUR_MAX_CLIQUE_SIZE = 0;
        max_degree = 0;
        CLIQUE_LIMIT = 0xFFFF;
    }

    nwgraph::nwgraph(const u64 n_vert, const u64 n_edges,
                     std::pair<std::vector<u64>, std::vector<u64>>&& edges,
                     std::vector<double>&& weights)
        : nwgraph()
    {
        clean_edges(n_vert + 1, edges);
        this->n_vert = n_vert + 1;
        this->vertices.resize(this->n_vert);

        this->edge_list.swap(edges.second);
        this->edge_bits.swap(edges.first);

        u64 i, j, spos, el_size_max = edge_list.size();
        u64* eb_base = edge_bits.data();
        double degree;

        for (i = 0; i < this->n_vert; i++)
        {
            spos = 0;
            degree = 0;
            for (j = 0; el_size + j < el_size_max && edge_bits[el_size + j] == i; j++)
            {
                edge_bits[el_size + j] = 0;
                degree += weights[edge_list[el_size + j]];
                if (edge_list[el_size + j] == i) spos = j;
            }

            this->vertices[i].load(j, this->el_size, this->eb_size, eb_base, spos,
                                   weights[i], degree);
            this->el_size += j;
            this->eb_size += (j % BITS_PER_U64 != 0) + j / BITS_PER_U64;
            if (degree > max_degree) max_degree = degree;
            if (j > max_neighbors) max_neighbors = j;
        }

        search_start = 1 + eb_size;
        search_cur = search_start;
        search_end = edge_bits.size();

        this->set_bounds();
    }

    /* call set_spos() for each vertex of the nwgraph, collect rudimentary
     * heuristics like maximum degree and maximum clique size for each vertex,
     * and ensure adequate memory is present for the clique search
     */
    void nwgraph::set_bounds()
    {
        u64 cur, j, vert, depth, size = vertices.size();
        double mcs = 0;

        max_depth = 0;
        CLIQUE_LIMIT = 0;

        // this loop doesn't benefit from being parallelized
        // because the CLIQUE_LIMIT check requires atomics
        for (cur = 0; cur < size; cur++)
        {
            mcs = 0;
            depth = 0;
            for (j = 0; j < this->vertices[cur].N; j++)
            {
                vert = edge_list[vertices[cur].elo + j];
                // degree of vert has to be >= degree of cur
                // to be considered while searching from cur
                if (vertices[vert].degree >= vertices[cur].degree)
                    mcs += vertices[vert].weight;
                if (vertices[vert].N >= vertices[cur].N) depth += 1;
            }
            vertices[cur].mcs = mcs;
            if (mcs > CLIQUE_LIMIT)
            {
                CUR_MAX_CLIQUE_LOCATION = cur;
                CLIQUE_LIMIT = mcs;
            }
            if (depth > max_depth) max_depth = depth;
        }

        const u64 size_per_step =
            (max_neighbors % BITS_PER_U64 != 0) + max_neighbors / BITS_PER_U64;
        const u64 spread = (search_end - search_start);
        const u64 max_space = 2 * (max_depth + 1) * (size_per_step);
        /*
         * Available memory in edge_bits ~= (|V| + 2|E|) - ceil(2|E|/64) max.
         * degree for a vertex = d_max < |V|
         * -> max. clique search depth = d_max < |V|
         * -> max. search space required = d_max x ceil(d_max/64) (dividing by
         *  64-bit for bitsets to be safe) Claim is that available memory is
         *  almost always greater than maximum search space required.
         *
         *  (assume that search space is reusable)
         *
         * the condition in the below line is almost always false, due to
         * the above calculation (may happen for really small or dense nwgraphs)
         */
        if (spread < max_space + 1)
        {
            std::cerr << "search spread: " << spread
                      << "; max requirement: " << max_space << "; ratio = "
                      << (1.0 * (search_end - search_start)) / (max_space) << std::endl;
            edge_bits.resize(eb_size + max_space + 1);
            search_end = edge_bits.size();
        }
    }

    void nwgraph::disp() const
    {
        for (u64 i = 0; i < this->n_vert; i++)
            this->vertices[i].disp(i, this->edge_list.data());
    }

    // pass edges one by one to external function
    void nwgraph::send_data(std::function<void(u64, u64)> dfunc) const
    {
        for (u64 i = 0; i < this->n_vert; i++)
        {
            for (u64 k = this->vertices[i].spos + 1; k < this->vertices[i].N; k++)
                dfunc(i, this->edge_list[this->vertices[i].elo + k]);
        }
    }

    // return all neighbors of a vertex as a set
    std::set<u64> nwgraph::vertex_data(u64 i) const
    {
        auto ans = std::set<u64>(
            this->edge_list.begin() + this->vertices[i].elo,
            this->edge_list.begin() + this->vertices[i].elo + this->vertices[i].N);
        ans.erase(ans.find(i));
        return ans;
    }

}  // namespace detail
}  // namespace cliquematch
