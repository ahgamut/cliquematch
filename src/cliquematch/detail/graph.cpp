/* graph.cpp
 *
 * Provides methods for the constructing a graph instance and exporting its
 * internal data.
 */
#include <detail/graph.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    /* I use u64 everywhere, so I know I am definitely getting a 64bit value
     * to work with during any calculation.
     */

    /* Insertion sort performed on a pair of arrays simultaneously.
     * Results in the pair (p0[i], p1[i]) sorted in increasing order
     * from start to end
     */
    void insertionSort(u64* p0, u64* p1, u64 start, u64 end)
    {
        for (u64 i = start + 1; i < end; i++)
        {
            for (u64 j = i; j != start && (p0[j - 1] != p0[j] ? p0[j - 1] > p0[j]
                                                              : p1[j - 1] > p1[j]);
                 j--)
            {
                std::swap(p0[j], p0[j - 1]);
                std::swap(p1[j], p1[j - 1]);
            }
        }
    }

    /* top-down radix sort performed on a pair of vectors simultaneously
     * called recursively downgrades to insertion sort when required
     * uses 4 bits i.e. 16 bins for sorting
     * Results in the pair (p0[i], p1[i]) sorted in increasing order
     * from start to end
     */
    void radixSort(u64* p0, u64* p1, u64 start, u64 end, u64 shift = 0,
                   bool key_first = true, const u64 max_shift = 28)
    {
        u64 i, j, k;
        u64 starts[16] = {0};
        u64 ends[16] = {0};
        auto p = key_first ? p0 : p1;

        for (i = start; i < end; i++) ends[(p[i] >> shift) & 0xF]++;
        starts[0] = start;
        ends[0] += start;
        for (i = 1; i < 16; i++)
        {
            starts[i] = ends[i - 1];
            ends[i] = ends[i] + ends[i - 1];
        }

        for (i = 0; i < 16; i++)
        {
            for (j = starts[i]; j < ends[i];)
            {
                k = (p[j] >> shift) & 0xF;
                if (k == i)
                    j++;
                else
                {
                    std::swap(p0[j], p0[starts[k]]);
                    std::swap(p1[j], p1[starts[k]]);
                    starts[k]++;
                }
            }
        }

        if (shift == 0)
        {
            if (!key_first) return;
            shift = max_shift;
            key_first = false;
        }
        else
            shift -= 4;

        starts[0] = start;
        for (i = 1; i < 16; i++) starts[i] = ends[i - 1];
        // parallelization gives 2x speedup
        // #pragma omp parallel for
        for (i = 0; i < 16; i++)
        {
            if (starts[i] >= ends[i])
                continue;
            else if (ends[i] - starts[i] <= 32)
                insertionSort(p0, p1, starts[i], ends[i]);
            else
                radixSort(p0, p1, starts[i], ends[i], shift, key_first, max_shift);
        }
    }

    /* modification of std::unique implemented for a pair of vectors
     * if (p0[i] == p0[j] && p1[i] == p1[j]) then p0[j], p1[j] are removed
     * CALLER has to resize the array after completion
     */
    u64 modUnique(u64* p0, u64* p1, u64 start, u64 end)
    {
        if (start == end) return end;
        u64 result = start;
        while (++start != end)
        {
            if ((p0[result] != p0[start] || p1[result] != p1[start]) &&
                ++result != start)
            {
                p0[result] = p0[start];
                p1[result] = p1[start];
            }
        }
        return ++result;
    }

    /* sort edges in increasing order and remove duplicates
     * the custom radixSort+unique is faster than std::sort+unique on a vector<pair>,
     * not to mention the memory savings in the graph constructor
     */
    void clean_edges(u64 n_vert, std::pair<std::vector<u64>, std::vector<u64>>& edges)
    {
        u64 m = n_vert, radix_shift = 0;
        for (; m != 0; m >>= 4) radix_shift += 4;
        radix_shift -= 4;
        radixSort(edges.first.data(), edges.second.data(), 0, edges.first.size(),
                  radix_shift, true, radix_shift);
        m = modUnique(edges.first.data(), edges.second.data(), 0, edges.first.size());
        edges.first.resize(m);
        edges.second.resize(m);
    }

    graph::graph()
    {
        n_vert = 0;
        el_size = 0;
        eb_size = 0;
        max_degree = 0;
        search_start = 0;
        search_end = 0;
        CUR_MAX_CLIQUE_LOCATION = 0;
        CUR_MAX_CLIQUE_SIZE = 0;
        CLIQUE_LIMIT = 0xFFFF;
    }

    graph::graph(const u64 n_vert, const u64 n_edges,
                 std::pair<std::vector<u64>, std::vector<u64>>&& edges)
        : graph()
    {
        clean_edges(n_vert + 1, edges);
        this->n_vert = n_vert + 1;
        this->vertices.resize(this->n_vert);

        this->edge_list.swap(edges.second);
        this->edge_bits.swap(edges.first);
        /* this is why a pair<vector>&& is used instead of pair<vector>& or
         * vector<pair>&. Instead of allocating yet another large block of
         * memory and copying the edge info there, I can simply swap the memory
         * into the vectors owned by the graph. (This necessitated the custom
         * sort+unique for pair<vector>s instead of std::sort+unique.)
         *
         * Note also that edge_bits holds way more memory than required; I can
         * use the extra memory during the search via load_memory() and
         * clear_memory() instead of making repeated calls to the heap for
         * small allocations.
         *
         * edge_bits is vector<u64>. no need to reinterpret_cast<u64*> to use this
         * memory.
         */

        u64 i, j, spos, el_size_max = edge_list.size();
        u64* eb_base = edge_bits.data();
        for (i = 0; i < this->n_vert; i++)
        {
            spos = 0;
            for (j = 0; el_size + j < el_size_max && edge_bits[el_size + j] == i; j++)
            {
                edge_bits[el_size + j] = 0;
                if (edge_list[el_size + j] == i) spos = j;
            }

            this->vertices[i].refer_from(j, this->el_size, this->eb_size);
            this->vertices[i].set_spos(eb_base, spos);
            this->el_size += j;
            this->eb_size += (j % BITS_PER_U64 != 0) + j / BITS_PER_U64;
            if (j > max_degree) max_degree = j;
        }

        search_start = 1 + eb_size;
        search_cur = search_start;
        search_end = edge_bits.size();

        this->set_bounds();
    }

    /* call set_spos() for each vertex of the graph, collect rudimentary
     * heuristics like maximum degree and maximum clique size for each vertex,
     * and ensure adequate memory is present for the clique search
     */
    void graph::set_bounds()
    {
        u64 cur, j, vert, mcs, size = vertices.size();
        CLIQUE_LIMIT = 0;

        // this loop doesn't benefit from being parallelized
        // because the CLIQUE_LIMIT check requires atomics
        for (cur = 0; cur < size; cur++)
        {
            mcs = 0;
            for (j = 0; j < this->vertices[cur].spos; j++)
            {
                // vert always has greater id than cur
                vert = edge_list[vertices[cur].elo + j];
                // degree of vert has to be >= degree of cur
                // to be considered while searching from cur
                mcs += (vertices[vert].N > vertices[cur].N);
            }
            for (; j < this->vertices[cur].N; j++)
            {
                // vert always has greater id than cur
                vert = edge_list[vertices[cur].elo + j];
                // degree of vert has to be >= degree of cur
                // to be considered while searching from cur
                mcs += (vertices[vert].N >= vertices[cur].N);
            }
            vertices[cur].mcs = mcs;
            if (mcs > CLIQUE_LIMIT)
            {
                CUR_MAX_CLIQUE_LOCATION = cur;
                CLIQUE_LIMIT = mcs;
            }
        }

        const u64 size_per_step =
            (max_degree % BITS_PER_U64 != 0) + max_degree / BITS_PER_U64;
        const u64 spread = (search_end - search_start);
        const u64 max_space = 2 * (CLIQUE_LIMIT + 1) * (size_per_step);
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
         * the above calculation (may happen for really small or dense graphs)
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

    void graph::disp() const
    {
        for (u64 i = 0; i < this->n_vert; i++)
            this->vertices[i].disp(i, this->edge_list.data());
    }

    // pass edges one by one to external function
    void graph::send_data(std::function<void(u64, u64)> dfunc) const
    {
        for (u64 i = 0; i < this->n_vert; i++)
        {
            for (u64 k = this->vertices[i].spos + 1; k < this->vertices[i].N; k++)
                dfunc(i, this->edge_list[this->vertices[i].elo + k]);
        }
    }

    // return all neighbors of a vertex as a set
    std::set<u64> graph::vertex_data(u64 i) const
    {
        auto ans = std::set<u64>(
            this->edge_list.begin() + this->vertices[i].elo,
            this->edge_list.begin() + this->vertices[i].elo + this->vertices[i].N);
        ans.erase(ans.find(i));
        return ans;
    }

    std::pair<std::vector<u64>, std::vector<u64>> iso_edges(u64& num_vertices,
                                                            u64& num_edges,
                                                            const graph& g1,
                                                            const graph& g2)
    {
        // subtract 1 from each because 0 is a sentinel vertex for graph
        num_vertices = (g1.n_vert - 1) * (g2.n_vert - 1);
        num_edges = 0;
        std::pair<std::vector<u64>, std::vector<u64>> edges;
        edges.first.resize(num_vertices + 1);
        edges.second.resize(num_vertices + 1);

        u64 i1, i2, j1, j2, v1, v2;
        u64 k, l;
        BFResult f1, f2;

        for (l = 0; l < edges.first.size(); l++)
        {
            edges.first[l] = l;
            edges.second[l] = l;
        }
        for (i1 = 1; i1 < g1.n_vert; ++i1)
        {
            // undirected edges, so symmetric
            for (i2 = i1 + 1; i2 < g1.n_vert; i2++)
            {
                f1 = g1.find_if_neighbors(i1, i2, k);
                for (j1 = 1; j1 < g2.n_vert; ++j1)
                {
                    // undirected edges, so symmetric
                    for (j2 = j1 + 1; j2 < g2.n_vert; ++j2)
                    {
                        f2 = g2.find_if_neighbors(j1, j2, l);
                        // g2 is a subgraph of g1, which means
                        // an edge (j1, j2) in g2 => there exists an edge (i1, i2) in g1
                        // (x => y means ~x | y)
                        if ((f1 != 1) && (f2 == 1)) continue;

                        // encode vertex info as i*M + j
                        // assume j1 maps to i1, j2 maps to i2
                        v1 = (i1 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                        v2 = (i2 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                        edges.first.push_back(v1);
                        edges.second.push_back(v2);
                        edges.first.push_back(v2);
                        edges.second.push_back(v1);

                        // undirected edges, so j1 may map to i2 also
                        v1 = (i2 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                        v2 = (i1 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                        edges.first.push_back(v1);
                        edges.second.push_back(v2);
                        edges.first.push_back(v2);
                        edges.second.push_back(v1);
                        num_edges += 2;
                    }
                }
            }
        }
        return edges;
    }
}  // namespace detail
}  // namespace cliquematch
