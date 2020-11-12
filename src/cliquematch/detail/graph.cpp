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
    /* I use std::size_t everywhere except for when loading a graphBits instance,
     * where I need to know I am definitely getting a 32bit value to work with.
     * The assertion is just to know that a reinterpret_cast<u32*>(std::size_t*)
     * isn't going to cause errors
     */
    static_assert(sizeof(std::size_t) >= sizeof(u32),
                  "cannot pack 32bit values into std::size_t");
    constexpr std::size_t U32_PER_SIZE_T = sizeof(std::size_t) / sizeof(u32);

    /* Insertion sort performed on a pair of arrays simultaneously.
     * Results in the pair (p0[i], p1[i]) sorted in increasing order
     * from start to end
     */
    void insertionSort(std::size_t* p0, std::size_t* p1, std::size_t start,
                       std::size_t end)
    {
        for (std::size_t i = start + 1; i < end; i++)
        {
            for (std::size_t j = i;
                 j != start &&
                 (p0[j - 1] != p0[j] ? p0[j - 1] > p0[j] : p1[j - 1] > p1[j]);
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
    void radixSort(std::size_t* p0, std::size_t* p1, std::size_t start, std::size_t end,
                   std::size_t shift = 0, bool key_first = true,
                   const std::size_t max_shift = 28)
    {
        std::size_t i, j, k;
        std::size_t starts[16] = {0};
        std::size_t ends[16] = {0};
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
    std::size_t modUnique(std::size_t* p0, std::size_t* p1, std::size_t start,
                          std::size_t end)
    {
        if (start == end) return end;
        std::size_t result = start;
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
    void clean_edges(
        std::size_t n_vert,
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>>& edges)
    {
        std::size_t m = n_vert, radix_shift = 0;
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
        md_vert = 0;
        search_start = 0;
        search_end = 0;
        CUR_MAX_CLIQUE_LOC = 0;
        CUR_MAX_CLIQUE_SIZE = 0;
        CLIQUE_LIMIT = 0xFFFF;
    }

    graph::graph(const std::size_t n_vert, const std::size_t n_edges,
                 std::pair<std::vector<std::size_t>, std::vector<std::size_t>>&& edges)
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
         * edge_bits is vector<size_t> to enable the swap. Earlier it was vector<u32>.
         * So I have to use reinterpret_cast<u32*> to use this memory.
         */

        std::size_t i, j;
        for (i = 0; i < this->n_vert; i++)
        {
            for (j = 0; el_size + j < edge_list.size() && edge_bits[el_size + j] == i;
                 j++)
                edge_bits[el_size + j] = 0;

            this->vertices[i].refer_from(i, j, this->el_size, this->eb_size);
            this->el_size += j;
            this->eb_size += (j % BITS_PER_SIZE_T != 0) + j / BITS_PER_SIZE_T;
        }

        search_start = 1 + eb_size;
        search_cur = search_start;
        search_end = edge_bits.size();

        this->set_vertices();
    }

    /* call set_spos() for each vertex of the graph, collect rudimentary
     * heuristics like maximum degree and maximum clique size for each vertex,
     * and esnure adequate memory is present for the clique search
     */
    void graph::set_vertices()
    {
        max_degree = CLIQUE_LIMIT = 0;
        std::size_t cur, j, vert;
        // this loop may benefit from being parallelized
        for (cur = 0; cur < vertices.size(); cur++)
        {
            vertices[cur].set_spos(this->edge_list.data(),
                                   reinterpret_cast<u32*>(this->edge_bits.data()));
            for (j = this->vertices[cur].spos + 1; j < this->vertices[cur].N; j++)
            {
                // vert always has greater id than cur
                vert = edge_list[vertices[cur].elo + j];
                // degree of vert has to be >= degree of cur
                // to be considered while searching from cur
                vertices[cur].mcs += (vertices[vert].N >= vertices[cur].N);
                // degree of vert has to be < degree of cur
                // to be considered while searching from vert
                vertices[vert].mcs += (vertices[vert].N < vertices[cur].N);
            }
            if (vertices[cur].mcs > CLIQUE_LIMIT)
            {
                md_vert = cur;
                CLIQUE_LIMIT = vertices[cur].mcs;
            }
            if (vertices[cur].N > max_degree) max_degree = vertices[cur].N;
        }

        const std::size_t size_per_step =
            (max_degree % BITS_PER_SIZE_T != 0) + max_degree / BITS_PER_SIZE_T;
        const std::size_t spread = (search_end - search_start);
        const std::size_t max_space = 2 * (CLIQUE_LIMIT + 1) * (size_per_step);
        /*
         * Available memory in edge_bits ~= (|V| + 2|E|) - ceil(2|E|/64) max.
         * degree for a vertex = d_max < |V|
         * -> max. clique search depth = d_max < |V|
         * -> max. search space required = d_max x ceil(d_max/64) (dividing by
         *  64-bit for bitsets to be safe) Claim is that available memory is
         *  almost always greater than max. search space required. (reusable
         *  search space)
         *
         * the condition in the below line is almost always false, due to
         * the above calculation (may happen for really small or dense graphs)
         */
        if (spread < max_space + 1)
        {
            std::cout << "search spread: " << spread
                      << "; max requirement: " << max_space << "; ratio = "
                      << (1.0 * (search_end - search_start)) / (max_space) << std::endl;
            edge_bits.resize(eb_size + max_space + 1);
            search_end = edge_bits.size();
        }
    }

    void graph::disp() const
    {
        for (std::size_t i = 0; i < this->n_vert; i++)
            this->vertices[i].disp(this->edge_list.data());
    }

    // pass edges one by one to external function
    void graph::send_data(std::function<void(std::size_t, std::size_t)> dfunc) const
    {
        for (std::size_t i = 0; i < this->n_vert; i++)
        {
            for (std::size_t k = this->vertices[i].spos + 1; k < this->vertices[i].N;
                 k++)
                dfunc(i, this->edge_list[this->vertices[i].elo + k]);
        }
    }

    // return all neighbors of a vertex as a set
    std::set<std::size_t> graph::vertex_data(std::size_t i) const
    {
        auto ans = std::set<std::size_t>(
            this->edge_list.begin() + this->vertices[i].elo,
            this->edge_list.begin() + this->vertices[i].elo + this->vertices[i].N);
        ans.erase(ans.find(i));
        return ans;
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> iso_edges(
        std::size_t& num_vertices, std::size_t& num_edges, const graph& g1,
        const graph& g2)
    {
        // subtract 1 from each because 0 is a sentinel vertex for graph
        num_vertices = (g1.n_vert - 1) * (g2.n_vert - 1);
        num_edges = 0;
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>> edges;
        edges.first.resize(num_vertices + 1);
        edges.second.resize(num_vertices + 1);

        std::size_t i1, i2, j1, j2, v1, v2;
        std::size_t k, l;
        short f1, f2;

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
