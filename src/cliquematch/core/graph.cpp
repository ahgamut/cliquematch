#include <core/graph.h>
#include <algorithm>
#include <iostream>

using namespace std;

void clean_edges(vector<pair<std::size_t, std::size_t>>& edges)
{
    std::sort(edges.begin(), edges.end());
    auto it = std::unique(edges.begin(), edges.end());
    edges.resize(std::distance(edges.begin(), it));
}

short graph::find_if_neighbors(std::size_t v1_id, std::size_t v2_id,
                               std::size_t& v2_position) const
{
    return binary_find(&(this->edge_list[this->vertices[v1_id].elo]),
                       this->vertices[v1_id].N, v2_id, v2_position);
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
    CUR_MAX_CLIQUE_LOC = 0;
    CUR_MAX_CLIQUE_SIZE = 0;
    CLIQUE_LIMIT = 0xFFFF;
}

graph::graph(std::size_t n_vert, std::size_t n_edges, vector<set<std::size_t>>& edges)
    : graph()
{
    this->n_vert = n_vert + 1;
    // Therefore the 0th graph vertex is always a sentinel, remember the offset
    this->vertices = vector<vertex>(this->n_vert);
    this->edge_list = vector<std::size_t>(this->n_vert + 2 * n_edges);

    for (std::size_t i = 0; i < edges.size(); i++)
    {
        edges[i].insert(i);
        std::copy(edges[i].begin(), edges[i].end(), this->edge_list.begin() + el_size);
        this->vertices[i].load_external(i, edges[i].size(), el_size, eb_size);
        this->max_degree = max_degree > edges[i].size() ? max_degree : edges[i].size();
        this->el_size += edges[i].size();
        this->eb_size += (edges[i].size() % 32 != 0) + edges[i].size() / 32;
        edges[i].erase(i);
    }

    this->edge_bits = vector<u32>(eb_size);
    this->set_vertices();
}

graph::graph(std::size_t n_vert, std::size_t n_edges,
             vector<pair<std::size_t, std::size_t>>& edges)
    : graph()
{
    clean_edges(edges);
    this->n_vert = n_vert + 1;
    this->vertices = vector<vertex>(this->n_vert);
    this->edge_list = vector<std::size_t>(edges.size());

    std::size_t i, j;
    for (i = 0; i < this->n_vert; i++)
    {
        for (j = 0; el_size + j < edges.size() && edges[el_size + j].first == i; j++)
            this->edge_list[this->el_size + j] = edges[this->el_size + j].second;
        this->vertices[i].load_external(i, j, this->el_size, this->eb_size);
        if (this->max_degree < j)
        {
            this->max_degree = j;
            this->md_vert = i;
        }
        this->el_size += j;
        this->eb_size += (j % 32 != 0) + j / 32;
    }

    this->edge_bits = vector<u32>(eb_size);
    this->set_vertices();
}

void graph::set_vertices()
{
    for (std::size_t i = 0; i < vertices.size(); i++)
        vertices[i].set_spos(this->edge_list.data(), this->edge_bits.data());
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
        for (std::size_t k = this->vertices[i].spos + 1; k < this->vertices[i].N; k++)
            dfunc(i, this->edge_list[this->vertices[i].elo + k]);
    }
}

std::vector<std::pair<std::size_t, std::size_t>> iso_edges(std::size_t& num_vertices,
                                                           std::size_t& num_edges,
                                                           const graph& g1,
                                                           const graph& g2)
{
    num_vertices = (g1.n_vert - 1) * (g2.n_vert - 1);
    num_edges = 0;
    std::vector<std::pair<std::size_t, std::size_t>> edges(num_vertices + 1);

    std::size_t i1, i2, j1, j2, v1, v2;
    std::size_t k, l;
    short f1, f2;

    for (l = 0; l < edges.size(); l++) edges[l] = {l, l};
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
                    edges.push_back(std::make_pair(v1, v2));
                    edges.push_back(std::make_pair(v2, v1));

                    v1 = (i2 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                    v2 = (i1 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                    edges.push_back(std::make_pair(v1, v2));
                    edges.push_back(std::make_pair(v2, v1));

                    num_edges += 2;
                }
            }
        }
    }
    return edges;
}
