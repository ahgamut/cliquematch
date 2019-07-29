#include <core/graph.h>
#include <algorithm>
#include <ctime>
#include <iostream>

using namespace std;

graph::graph() {
    this->n_vert = 0;
    this->el_size = 0;
    this->eb_size = 0;
    this->max_degree = 0;
    this->duration = 0.0;

    this->vertices = vector<vertex>();
    this->edge_list = vector<u32>();
    this->edge_bits = vector<u32>();

    this->V = nullptr;
    this->el_base = nullptr;
    this->eb_base = nullptr;

    this->CUR_MAX_CLIQUE_SIZE = 1;
    this->CUR_MAX_CLIQUE_LOC = 0;
    this->CLIQUE_LIMIT = 1;
    this->TIME_LIMIT = 100;
}

graph::graph(u32 n_vert, u32 n_edges, std::vector<std::set<u32> >& edges,
             u32 clique_lim) {
    this->n_vert = n_vert + 1;
    // Therefore the 0th graph vertex is always a sentinel, remember the offset
    this->el_size = 0;
    this->eb_size = 0;
    this->max_degree = 0;
    this->duration = 0.0;
    this->CLIQUE_LIMIT = clique_lim;
    this->CUR_MAX_CLIQUE_SIZE = 1;
    this->CUR_MAX_CLIQUE_LOC = 0;
    this->TIME_LIMIT = 100;

    this->vertices = vector<vertex>(this->n_vert);
    this->edge_list = vector<u32>();

    for (u32 i = 0; i < edges.size(); i++) {
        edges[i].insert(i);
        this->vertices[i] = vertex(i, edges[i].size(), el_size, eb_size);
        this->edge_list.insert(this->edge_list.end(), edges[i].begin(),
                               edges[i].end());
        this->max_degree =
            max_degree > edges[i].size() ? max_degree : edges[i].size();
        this->el_size += edges[i].size();
        this->eb_size += 1 + edges[i].size() / 32;
        edges[i].erase(i);
    }

    this->edge_bits = vector<u32>(eb_size);
    this->el_base = this->edge_list.data();
    this->V = this->vertices.data();
    this->eb_base = this->edge_bits.data();
    this->set_vertices();
}

void graph::set_vertices() {
    for (u32 i = 0; i < vertices.size(); i++)
        vertices[i].set_spos(this->el_base, this->eb_base);
}

void graph::disp() {
    for (u32 i = 0; i < this->n_vert; i++)
        this->vertices[i].disp(this->el_base);
}

void graph::find_max_cliques(u32& start_vert, bool& heur_done, bool use_heur,
                             bool use_dfs, double time_limit) {
    if (start_vert != 0) {
        std::cerr << "Continuing at " << start_vert
                  << " off of a previous search ";
        if (!heur_done)
            std::cerr << "(heuristic)\n";
        else
            std::cerr << "(DFS)\n";
    }
    duration = clock();
    // I'm not sorting by degree because locality (?)
    if (!heur_done && use_heur)
        start_vert = heur_all_cliques(start_vert, time_limit);

    if ((clock() - duration) / CLOCKS_PER_SEC < time_limit) {
        if (!heur_done) start_vert = 0;
        heur_done = true;
        if (use_dfs) start_vert = dfs_all_cliques(start_vert, time_limit);
    }

    duration = (clock() - duration) / ((double)CLOCKS_PER_SEC);
}

vector<u32> graph::get_max_clique() {
    return this->get_max_clique(this->CUR_MAX_CLIQUE_LOC);
}

vector<u32> graph::get_max_clique(u32 i) {
    return this->vertices[i].give_clique(el_base);
}