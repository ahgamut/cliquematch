#include <core/pygraph.h>
#include <core/mmio.h>
#include <core/graph.h>
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
        use_heur = false;
        use_dfs = true;
        lower_bound = 1;
        upper_bound = 0xFFFFFFFF;
        time_lim = 100.0;
        current_vertex = 0;
        finished_heur = false;
        finished_all = false;
        ans_found = false;
        inited = false;
    }
    void pygraphDeleter::operator()(pygraph* pg)
    {
        if (pg->inited) delete pg->G;
        pg->inited = false;
        delete pg;
    }
    void pygraph::load_graph(std::size_t n_vertices, std::size_t n_edges,
                             std::vector<std::set<std::size_t>> edges)
    {
        this->nvert = n_vertices;
        this->nedges = n_edges;
        if (this->inited) delete this->G;
        this->G = new detail::graph(this->nvert, this->nedges, edges);
        this->inited = true;
    }
    void pygraph::load_graph(std::size_t n_vertices, std::size_t n_edges,
                             std::vector<std::pair<std::size_t, std::size_t>> edges)
    {
        this->nvert = n_vertices;
        this->nedges = n_edges;
        if (this->inited) delete this->G;
        this->G = new detail::graph(this->nvert, this->nedges, edges);
        this->inited = true;
    }

    // Computation

    void pygraph::find_max_clique()
    {
        if (this->G->n_vert == 0) throw CM_ERROR("Graph is not initialized!!\n");

        //	std::cerr<<"Finding cliques\n";
        this->G->CUR_MAX_CLIQUE_SIZE = this->lower_bound > this->G->CUR_MAX_CLIQUE_SIZE
                                           ? this->lower_bound
                                           : this->G->CUR_MAX_CLIQUE_SIZE;
        this->G->CLIQUE_LIMIT = this->upper_bound < this->G->max_degree
                                    ? this->upper_bound
                                    : this->G->max_degree;
        this->G->find_max_cliques(current_vertex, finished_heur, use_heur, use_dfs,
                                  time_lim);
        ans_clique = this->G->get_max_clique();
        ans_found = true;
        finished_all = finished_heur && (current_vertex >= nvert);
    }
    // Finds the maximum clique and returns it as a std::vector
    std::vector<std::size_t> pygraph::get_max_clique()
    {
        if (!ans_found) find_max_clique();
        if (this->lower_bound > this->ans_clique.size() ||
            this->G->CUR_MAX_CLIQUE_LOC == 0)
            throw CM_ERROR("Unable to find maximum clique with given bounds\n");
        return this->ans_clique;
    }
    void pygraph::continue_search()
    {
        if (!finished_all)
        {
            ans_found = false;
            find_max_clique();
        }
        else
        {
            std::cerr << "Search is complete.\n";
        }
    }
    //' The clique search can be reset in case the current set of parameters were
    // not optimal
    void pygraph::reset_search()
    {
        this->ans_found = false;
        this->finished_all = false;
        this->finished_heur = false;
        this->current_vertex = 0;
        this->G->CUR_MAX_CLIQUE_SIZE = 1;
        this->G->CUR_MAX_CLIQUE_LOC = 0;
    }

    // IO

    std::string pygraph::showdata()
    {
        std::stringstream ss;
        ss << "cliquematch.core.Graph object at " << this << "\n(";
        ss << "n_vertices=" << this->nvert << ",";
        ss << "n_edges=" << this->nedges << ",";
        ss << "search_done=" << (this->finished_all ? "True" : "False") << ",";
        ss << "lower_bound=" << this->lower_bound << ",";
        ss << "upper_bound=" << this->upper_bound << ",";
        ss << "time_limit=" << this->time_lim << ",";
        ss << "use_heuristic=" << (this->use_heur ? "True" : "False") << ",";
        ss << "use_dfs=" << (this->use_dfs ? "True" : "False");
        ss << ")";
        return ss.str();
    }

    pygraph from_file(std::string filename)
    {
        std::size_t no_of_vertices, no_of_edges;
        pygraph pg;
        auto edges =
            detail::mmio3_reader(filename.c_str(), no_of_vertices, no_of_edges);

        if (edges.data() == NULL || edges.size() == 0)
            throw CM_ERROR("Could not extract edges!!\n");
        pg.load_graph(no_of_vertices, no_of_edges, edges);
        return pg;
    }

    pygraph from_edgelist(ndarray<std::size_t> edge_list1, std::size_t no_of_vertices)
    {
        //	std::cout<<"Constructing graph from the list of edges (Nx2 matrix)\n";
        std::size_t no_of_edges = 0;
        std::size_t v1, v2;
        auto edge_list = edge_list1.unchecked<2>();
        std::vector<std::pair<std::size_t, std::size_t>> edges(no_of_vertices + 1);
        pygraph pg;

        for (std::size_t i = 0; i < edges.size(); i++) edges[i] = {i, i};
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
            edges.push_back(std::make_pair(v1, v2));
            edges.push_back(std::make_pair(v2, v1));
            no_of_edges++;
        }

        if (edges.data() == NULL || edges.size() == 0)
            throw CM_ERROR("Could not extract edges!!\n");
        pg.load_graph(no_of_vertices, no_of_edges, edges);
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

            std::vector<std::pair<std::size_t, std::size_t>> edges;

            for (std::size_t i = 0; i < no_of_vertices; i++)
            {
                for (std::size_t j = 0; j < no_of_vertices; j++)
                {
                    if (adjmat(i, j) || i == j)
                    {
                        edges.push_back(std::make_pair(i + 1, j + 1));
                        edges.push_back(std::make_pair(j + 1, i + 1));
                        no_of_edges += (i != j);
                    }
                }
            }

            no_of_edges /= 2;
            if (edges.data() == NULL || edges.size() == 0)
                throw CM_ERROR("Could not extract edges!!\n");
            pg.load_graph(no_of_vertices, no_of_edges, edges);
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

        pygraph pg;
        for (std::size_t i = 0; i < edges.size(); i++)
        {
            for (auto& j : edges[i])
            {
                if (j == 0 || j > n_vertices || j == i ||
                    edges[j].find(i) == edges[j].end())
                    throw CM_ERROR("Invalid edge in adjacency list!\n");
                e++;
            }
        }
        if (e / 2 != n_edges) throw CM_ERROR("Number of edges don't match!\n");
        pg.load_graph(n_vertices, n_edges, edges);
        return pg;
    }

    ndarray<std::size_t> pygraph::to_edgelist()
    {
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

    void pygraph::to_file(std::string filename)
    {
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

    ndarray<bool> pygraph::to_adj_matrix()
    {
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

    std::vector<std::set<std::size_t>> pygraph::to_adj_list()
    {
        std::vector<std::set<std::size_t>> edges(this->nvert + 1);
        this->G->send_data([&edges](std::size_t i, std::size_t j) {
            edges[i].insert(j);
            edges[j].insert(i);
        });
        return edges;
    }

    // Subgraph isomorphisms
    std::vector<std::pair<std::size_t, std::size_t>> iso_edges(std::size_t& nv,
                                                               std::size_t& ne,
                                                               const pygraph& g1,
                                                               const pygraph& g2)
    {
        return iso_edges(nv, ne, *(g1.G), *(g2.G));
    }
}  // namespace core
}  // namespace cliquematch
