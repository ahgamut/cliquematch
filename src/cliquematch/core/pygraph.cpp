#include <detail/mmio.h>
#include <detail/graph.h>
#include <core/pygraph.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

namespace cliquematch
{
namespace core
{
    namespace py = pybind11;
    // Initialization and Destruction

    pygraph::pygraph()
    {
        nvert = 0;
        nedges = 0;
        current_vertex = 0;
        finished_all = false;
        inited = false;
        G = nullptr;
    }
    pygraph::~pygraph()
    {
        if (this->inited) delete this->G;
        this->inited = false;
        this->G = nullptr;
    }
    pygraph::pygraph(pygraph&& other)
    {
        this->nvert = other.nvert;
        this->nedges = other.nedges;
        this->current_vertex = other.current_vertex;
        this->finished_all = other.finished_all;
        this->inited = other.inited;
        this->G = other.G;
        other.inited = false;
    }
    void pygraph::load_graph(
        std::size_t n_vertices, std::size_t n_edges,
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>>&& edges)
    {
        this->nvert = n_vertices;
        this->nedges = n_edges;
        if (this->inited) delete this->G;
        this->G = new detail::graph(this->nvert, this->nedges, std::move(edges));
        this->inited = true;
    }
    void pygraph::check_loaded() const
    {
        if (!this->inited || this->nvert == 0)
            throw CM_ERROR("Graph is not initialized!!\n");
    }
    void pygraphDeleter::operator()(pygraph* pg) { delete pg; }

    // Computation
    void pygraph::reset_search()
    {
        this->G->CUR_MAX_CLIQUE_SIZE = 1;
        this->finished_all = false;
        this->current_vertex = 0;
    }

    std::vector<std::size_t> pygraph::get_max_clique(std::size_t lower_bound,
                                                     std::size_t upper_bound,
                                                     double time_limit,
                                                     bool use_heuristic, bool use_dfs,
                                                     bool continue_search)
    {
        if (!continue_search)
            current_vertex = 0;
        else if (current_vertex != 0)
            use_heuristic = false;

        check_loaded();
        this->G->CUR_MAX_CLIQUE_SIZE = lower_bound > this->G->CUR_MAX_CLIQUE_SIZE
                                           ? lower_bound
                                           : this->G->CUR_MAX_CLIQUE_SIZE;
        this->G->CLIQUE_LIMIT =
            upper_bound < this->G->CLIQUE_LIMIT ? upper_bound : this->G->CLIQUE_LIMIT;
        this->G->find_max_cliques(current_vertex, use_heuristic, use_dfs, time_limit);
        finished_all = (current_vertex >= nvert);
        auto ans = this->G->get_max_clique();
        if (lower_bound > ans.size())
            throw CM_ERROR("Unable to find maximum clique with given bounds\n");
        return ans;
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
    pygraph::get_correspondence2(std::size_t len1, std::size_t len2,
                                 std::vector<std::size_t> clique)
    {
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>> ans;
        std::size_t i, t1, t2;
        for (i = 0; i < clique.size(); i++)
        {
            if (clique[i] == 0)
            {
                throw CM_ERROR(
                    "Sentinel Value (0) appeared in clique. Increase the time "
                    "limit; if repeated, reset graph\n");
            }
            t1 = (clique[i] - 1) / len2;
            t2 = (clique[i] - 1) % len2;
            ans.first.push_back(t1);
            ans.second.push_back(t2);
        }
        return ans;
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
    pygraph::get_correspondence(std::size_t len1, std::size_t len2,
                                std::size_t lower_bound, std::size_t upper_bound,
                                double time_limit, bool use_heuristic, bool use_dfs,
                                bool continue_search)
    {
        return this->get_correspondence2(
            len1, len2,
            this->get_max_clique(lower_bound, upper_bound, time_limit, use_heuristic,
                                 use_dfs, continue_search));
    }
    // IO

    std::string pygraph::showdata() const
    {
        std::stringstream ss;
        ss << "cliquematch.core.Graph object at " << this << "(";
        ss << "n_vertices=" << this->nvert << ",";
        ss << "n_edges=" << this->nedges << ",";
        ss << "search_done=" << (this->current_vertex < this->nvert ? "True" : "False");
        ss << ")";
        return ss.str();
    }

    pygraph from_file(std::string filename)
    {
        std::size_t no_of_vertices, no_of_edges;
        pygraph pg;
        auto edges =
            detail::mmio4_reader(filename.c_str(), no_of_vertices, no_of_edges);

        if (edges.first.size() == 0 || edges.second.size() == 0)
            throw CM_ERROR("Could not extract edges!!\n");
        pg.load_graph(no_of_vertices, no_of_edges, std::move(edges));
        return pg;
    }

    pygraph from_edgelist(ndarray<std::size_t> edge_list1, std::size_t no_of_vertices)
    {
        //	std::cout<<"Constructing graph from the list of edges (Nx2 matrix)\n";
        std::size_t no_of_edges = 0;
        std::size_t v1, v2;
        auto edge_list = edge_list1.unchecked<2>();
        std::pair<std::vector<std::size_t>, std::vector<std::size_t>> edges;
        edges.first.resize(no_of_vertices + 1);
        edges.second.resize(no_of_vertices + 1);
        pygraph pg;

        for (std::size_t i = 0; i < edges.first.size(); i++)
        {
            edges.first[i] = i;
            edges.second[i] = i;
        }
        for (auto i = 0; i < edge_list.shape(0); i++)
        {
            v1 = edge_list(i, 0);
            v2 = edge_list(i, 1);
            if (v1 > no_of_vertices || v2 > no_of_vertices)
                throw CM_ERROR(
                    "Edge contains vertex ID > number of vertices specified\n");
            if (v1 == 0 || v2 == 0)
            {
                throw CM_ERROR(
                    "Vertex numbers must begin at 1, 0 is used as a sentinel value\n");
            }
            no_of_edges++;
            edges.first.push_back(v1);
            edges.second.push_back(v2);
            edges.first.push_back(v2);
            edges.second.push_back(v1);
        }

        if (edges.first.size() == 0 || edges.second.size() == 0)
            throw CM_ERROR("Could not extract edges!!\n");
        pg.load_graph(no_of_vertices, no_of_edges, std::move(edges));
        return pg;
    }

    pygraph from_adj_matrix(ndarray<bool> adjmat1)
    {
        //	std::cout<<"Constructing graph from the adjacency matrix\n";
        auto adjmat = adjmat1.unchecked<2>();
        if (adjmat.shape(0) != adjmat.shape(1))
            throw CM_ERROR("Adjacency matrix has to be a square matrix!!\n");
        else
        {
            std::size_t no_of_vertices = adjmat.shape(0);
            std::size_t no_of_edges = 0;
            pygraph pg;

            std::pair<std::vector<std::size_t>, std::vector<std::size_t>> edges;

            for (std::size_t i = 0; i < no_of_vertices; i++)
            {
                for (std::size_t j = 0; j < no_of_vertices; j++)
                {
                    if (adjmat(i, j) || i == j)
                    {
                        edges.first.push_back(i + 1);
                        edges.second.push_back(j + 1);
                        edges.first.push_back(j + 1);
                        edges.second.push_back(i + 1);
                        no_of_edges += (i != j);
                    }
                }
            }

            no_of_edges /= 2;
            if (edges.first.size() == 0 || edges.second.size() == 0)
                throw CM_ERROR("Could not extract edges!!\n");
            pg.load_graph(no_of_vertices, no_of_edges, std::move(edges));
            return pg;
        }
    }
    pygraph from_adj_list(std::size_t n_vertices, std::size_t n_edges,
                          std::vector<std::set<std::size_t>> edges)
    {
        std::size_t e = 0;
        if (edges.size() > 0 && !edges[0].empty())
            throw CM_ERROR("0 is used as sentinel value, set 0 should be empty\n");
        if (n_vertices != edges.size() - 1)
            throw CM_ERROR("Number of vertices don't match!\n");

        std::pair<std::vector<std::size_t>, std::vector<std::size_t>> edges2;
        pygraph pg;
        for (std::size_t i = 0; i < edges.size(); i++)
        {
            edges2.first.push_back(i);
            edges2.second.push_back(i);
            for (auto& j : edges[i])
            {
                if (j == 0 || j > n_vertices || j == i ||
                    edges[j].find(i) == edges[j].end())
                    throw CM_ERROR("Invalid edge in adjacency list!\n");

                edges2.first.push_back(i);
                edges2.second.push_back(j);
                e++;
            }
        }
        if (e / 2 != n_edges) throw CM_ERROR("Number of edges don't match!\n");
        pg.load_graph(n_vertices, n_edges, std::move(edges2));
        return pg;
    }

    ndarray<std::size_t> pygraph::to_edgelist() const
    {
        check_loaded();
        ndarray<std::size_t> elist1(this->nedges * 2);
        elist1.resize({this->nedges, static_cast<std::size_t>(2)});
        auto elist = elist1.mutable_unchecked<2>();
        std::size_t k = 0, N = this->nedges;
        this->G->send_data([&elist, &k, &N](std::size_t i, std::size_t j) {
            // I could use N for checking bounds
            elist(k, 0) = i;
            elist(k, 1) = j;
            k++;
        });
        return elist1;
    }

    void pygraph::to_file(std::string filename) const
    {
        check_loaded();
        std::ofstream f(filename, std::ios::out);
        if (!f.is_open())
        {
            throw CM_ERROR("Unable to open " + filename + "!!");
            return;
        }

        f << "%% generated by cliquematch\n";
        f << this->nvert << " ";
        f << this->nvert << " ";
        f << this->nedges << "\n";
        this->G->send_data(
            [&f](std::size_t i, std::size_t j) { f << i << " " << j << "\n"; });
        f.close();
    }

    ndarray<bool> pygraph::to_adj_matrix() const
    {
        check_loaded();
        ndarray<bool> adjmat1(this->nvert * this->nvert);
        adjmat1.resize({this->nvert, this->nvert});
        auto adjmat = adjmat1.mutable_unchecked<2>();
        // zero out while initializing ??
        for (std::size_t i = 0; i < this->nvert; i++)
            for (std::size_t j = 0; j < this->nvert; ++j) { adjmat(i, j) = false; }

        // internals are 1-indexed , so subtract by one
        // out of bounds can only happen if either is zero,
        // which means there is already an error elsewhere
        this->G->send_data([&adjmat](std::size_t i, std::size_t j) {
            adjmat(i - 1, j - 1) = true;
            adjmat(j - 1, i - 1) = true;
        });
        return adjmat1;
    }

    std::vector<std::set<std::size_t>> pygraph::to_adj_list() const
    {
        check_loaded();
        std::vector<std::set<std::size_t>> edges(this->nvert + 1);
        this->G->send_data([&edges](std::size_t i, std::size_t j) {
            edges[i].insert(j);
            edges[j].insert(i);
        });
        return edges;
    }

    std::set<std::size_t> pygraph::get_vertex_data(std::size_t i) const
    {
        if (i > this->nvert) throw CM_ERROR("Vertex index out of bounds");
        return this->G->vertex_data(i);
    }

    // Subgraph isomorphisms
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> iso_edges(
        std::size_t& nv, std::size_t& ne, const pygraph& g1, const pygraph& g2)
    {
        return iso_edges(nv, ne, *(g1.G), *(g2.G));
    }
}  // namespace core
}  // namespace cliquematch
