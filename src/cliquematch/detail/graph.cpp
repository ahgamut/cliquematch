#include <detail/graph.h>
#include <algorithm>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    static_assert(sizeof(std::size_t) >= sizeof(u32),
                  "cannot pack 32bit values into std::size_t");
    constexpr std::size_t U32_PER_SIZE_T = sizeof(std::size_t) / sizeof(u32);

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

    void insertionSort(std::size_t* p1, std::size_t start, std::size_t end)
    {
        for (std::size_t i = start + 1; i < end; i++)
        {
            for (std::size_t j = i; j != start && p1[j - 1] > p1[j]; j--)
                std::swap(p1[j], p1[j - 1]);
        }
    }

    void radixSort(std::size_t* p0, std::size_t* p1, std::size_t start, std::size_t end,
                   std::size_t shift = 0, bool key_first = true)
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

        /*std::cout << "Buckets: " << std::endl;
        for (i = 0; i < 16; i++)
            std::cout << i << ": " << starts[i] << " " << ends[i] << std::endl;*/

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

        /*std::cout << "After binning Buckets: " << std::endl;
        for (i = 0; i < 16; i++)
            std::cout << i << ": " << starts[i] << " " << ends[i] << std::endl;*/

        if (shift == 0)
        {
            if (!key_first) return;
            shift = 28;
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
            {
                if (key_first)
                    insertionSort(p0, p1, starts[i], ends[i]);
                else
                    insertionSort(p1, starts[i], ends[i]);
            }
            else
                radixSort(p0, p1, starts[i], ends[i], shift, key_first);
        }
    }

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

    void clean_edges2(
        std::size_t n_vert,
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>>& edges)
    {
        std::size_t m = n_vert, radix_shift = 0;
        for (; m != 0; m >>= 4, radix_shift += 4)
            ;
        radix_shift -= 4;
        radixSort(edges.first.data(), edges.second.data(), 0, edges.first.size(),
                  radix_shift, true);
        m = modUnique(edges.first.data(), edges.second.data(), 0, edges.first.size());
        edges.first.resize(m);
        edges.second.resize(m);
    }

    void clean_edges(std::size_t n_vert,
                     std::vector<std::pair<std::size_t, std::size_t>>& edges)
    {
        using sp = std::pair<std::size_t, std::size_t>;
        std::sort(edges.begin(), edges.end(), [](const sp& a, const sp& b) {
            return (a.first < b.first) || (a.first == b.first && a.second < b.second);
        });
        auto it = std::unique(edges.begin(), edges.end());
        edges.resize(std::distance(edges.begin(), it));
    }

    void graph::start_clock() { this->start_time = std::chrono::steady_clock::now(); }

    double graph::elapsed_time() const
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - this->start_time);
        return static_cast<double>(elapsed.count()) / 1e6;
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
                 std::vector<std::set<std::size_t>>& edges)
        : graph()
    {
        this->n_vert = n_vert + 1;
        // Therefore the 0th graph vertex is always a sentinel, remember the offset
        this->vertices.resize(this->n_vert);
        this->edge_list.resize(this->n_vert + 2 * n_edges);

        for (std::size_t i = 0; i < edges.size(); i++)
        {
            edges[i].insert(i);
            std::copy(edges[i].begin(), edges[i].end(),
                      this->edge_list.begin() + el_size);
            this->vertices[i].refer_from(i, edges[i].size(), el_size, eb_size);
            this->max_degree =
                max_degree > edges[i].size() ? max_degree : edges[i].size();
            this->el_size += edges[i].size();
            this->eb_size += (edges[i].size() % 32 != 0) + edges[i].size() / 32;
            edges[i].erase(i);
        }

        this->edge_bits.resize(1 + eb_size / U32_PER_SIZE_T);
        this->set_vertices();
    }

    graph::graph(const std::size_t n_vert, const std::size_t n_edges,
                 std::vector<std::pair<std::size_t, std::size_t>>& edges)
        : graph()
    {
        clean_edges(n_vert + 1, edges);
        this->n_vert = n_vert + 1;
        this->vertices.resize(this->n_vert);
        this->edge_list.resize(edges.size());

        std::size_t i, j;
        for (i = 0; i < this->n_vert; i++)
        {
            for (j = 0; el_size + j < edges.size() && edges[el_size + j].first == i;
                 j++)
                this->edge_list[this->el_size + j] = edges[this->el_size + j].second;
            this->vertices[i].refer_from(i, j, this->el_size, this->eb_size);
            if (this->max_degree < j)
            {
                this->max_degree = j;
                this->md_vert = i;
            }
            this->el_size += j;
            this->eb_size += (j % 32 != 0) + j / 32;
        }

        this->edge_bits.resize(1 + eb_size / U32_PER_SIZE_T);
        this->set_vertices();
    }

    graph::graph(const std::size_t n_vert, const std::size_t n_edges,
                 std::pair<std::vector<std::size_t>, std::vector<std::size_t>>&& edges)
        : graph()
    {
        clean_edges2(n_vert + 1, edges);
        this->n_vert = n_vert + 1;
        this->vertices.resize(this->n_vert);
        this->edge_list.swap(edges.second);
        this->edge_bits.swap(edges.first);

        std::size_t i, j;
        for (i = 0; i < this->n_vert; i++)
        {
            for (j = 0; el_size + j < edge_list.size() && edge_bits[el_size + j] == i;
                 j++)
                edge_bits[el_size + j] = 0;

            // this->edge_list[this->el_size + j] = edges[this->el_size + j].second;
            this->vertices[i].refer_from(i, j, this->el_size, this->eb_size);
            if (this->max_degree < j)
            {
                this->max_degree = j;
                this->md_vert = i;
            }
            this->el_size += j;
            this->eb_size += (j % 32 != 0) + j / 32;
        }

        // std::cout << "obtained size: " << edge_bits.size()
        //         << "\t resizing to: " << (1 + eb_size / U32_PER_SIZE_T) << std::endl;
        search_start = 1 + eb_size / U32_PER_SIZE_T;
        search_cur = search_start;
        search_end = edge_bits.size();
        this->set_vertices();
    }

    void graph::set_vertices()
    {
        for (std::size_t i = 0; i < vertices.size(); i++)
            vertices[i].set_spos(this->edge_list.data(),
                                 reinterpret_cast<u32*>(this->edge_bits.data()));
        this->CLIQUE_LIMIT = this->max_degree;
    }

    void graph::disp() const
    {
        for (std::size_t i = 0; i < this->n_vert; i++)
            this->vertices[i].disp(this->edge_list.data());
    }

    void graph::send_data(std::function<void(std::size_t, std::size_t)> dfunc) const
    {
        for (std::size_t i = 0; i < this->n_vert; i++)
        {
            for (std::size_t k = this->vertices[i].spos + 1; k < this->vertices[i].N;
                 k++)
                dfunc(i, this->edge_list[this->vertices[i].elo + k]);
        }
    }

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
            for (i2 = i1 + 1; i2 < g1.n_vert; i2++)
            {
                f1 = g1.find_if_neighbors(i1, i2, k);
                for (j1 = 1; j1 < g2.n_vert; ++j1)
                {
                    for (j2 = j1 + 1; j2 < g2.n_vert; ++j2)
                    {
                        f2 = g2.find_if_neighbors(j1, j2, l);
                        if ((f1 != 1) && (f2 == 1)) continue;

                        v1 = (i1 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                        v2 = (i2 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                        edges.first.push_back(v1);
                        edges.second.push_back(v2);
                        edges.first.push_back(v2);
                        edges.second.push_back(v1);

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
