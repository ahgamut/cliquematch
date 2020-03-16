#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

graph::graph()
{
    this->n_vert = 0;
    this->el_size = 0;
    this->eb_size = 0;
    this->max_degree = 0;
    this->duration = 0.0;

    this->vertices = vector<vertex>();
    this->edge_list = vector<size_t>();
    this->edge_bits = vector<u32>();

    this->CUR_MAX_CLIQUE_SIZE = 1;
    this->CUR_MAX_CLIQUE_LOC = 0;
    this->CLIQUE_LIMIT = 1;
    this->TIME_LIMIT = 100;
}

graph::graph(size_t n_vert, size_t n_edges, vector<set<size_t> >& edges,
             size_t clique_lim)
    : graph()
{
    this->n_vert = n_vert + 1;
    // Therefore the 0th graph vertex is always a sentinel, remember the offset
    this->CLIQUE_LIMIT = clique_lim;

    this->vertices = vector<vertex>(this->n_vert);
    this->edge_list = vector<size_t>();

    for (size_t i = 0; i < edges.size(); i++)
    {
        edges[i].insert(i);
        this->vertices[i].load_external(i, edges[i].size(), el_size, eb_size);
        this->edge_list.insert(this->edge_list.end(), edges[i].begin(), edges[i].end());
        this->max_degree = max_degree > edges[i].size() ? max_degree : edges[i].size();
        this->el_size += edges[i].size();
        this->eb_size += 1 + edges[i].size() / 32;
        edges[i].erase(i);
    }

    this->edge_bits = vector<u32>(eb_size);
    this->set_vertices();
}

void graph::set_vertices()
{
    for (size_t i = 0; i < vertices.size(); i++)
        vertices[i].set_spos(this->edge_list.data(), this->edge_bits.data());
}

void graph::disp()
{
    for (size_t i = 0; i < this->n_vert; i++)
        this->vertices[i].disp(this->edge_list.data());
}

void graph::find_max_cliques(size_t& start_vert, bool& heur_done, bool use_heur,
                             bool use_dfs, double time_limit)
{
#ifndef NDEBUG
    if (start_vert != 0)
    {
        cerr << "Continuing at " << start_vert << " off of a previous search ";
        if (!heur_done)
            cerr << "(heuristic)\n";
        else
            cerr << "(DFS)\n";
    }
#endif
    duration = clock();
    // I'm not sorting by degree because locality (?)
    if (!heur_done && use_heur) start_vert = heur_all_cliques(start_vert, time_limit);

    if ((clock() - duration) / CLOCKS_PER_SEC < time_limit)
    {
        if (!heur_done) start_vert = 0;
        heur_done = true;
        if (use_dfs) start_vert = dfs_all_cliques(start_vert, time_limit);
    }

    duration = (clock() - duration) / ((double)CLOCKS_PER_SEC);
}

size_t graph::dfs_all_cliques(size_t start_vertex, double time_limit)
{
    size_t i = start_vertex;
    TIME_LIMIT = time_limit;
    for (; i < vertices.size(); i++)
    {
        if ((clock() - duration) / CLOCKS_PER_SEC > TIME_LIMIT)
        {
#ifndef NDEBUG
            cerr << "DFS: Exceeded time limit of " << TIME_LIMIT << " seconds\n";
#endif
            break;
        }
        if (this->vertices[i].N <= CUR_MAX_CLIQUE_SIZE ||
            CUR_MAX_CLIQUE_SIZE > CLIQUE_LIMIT)
            continue;
        dfs_one_clique(i);
    }
    // If we pause midway, I want to know where we stopped
    return i;
}

vector<size_t> graph::get_max_clique()
{
    return this->get_max_clique(this->CUR_MAX_CLIQUE_LOC);
}

vector<size_t> graph::get_max_clique(size_t i)
{
    return this->vertices[i].give_clique(this->edge_list.data());
}
