#include <detail/nwgraph/nwgraph.h>
#include <core/pynwgraph.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace cliquematch
{
namespace core
{
    namespace py = pybind11;
    // Initialization and Destruction

    pynwgraph::pynwgraph()
    {
        nvert = 0;
        nedges = 0;
        current_vertex = 0;
        finished_all = false;
    }
    void pynwgraph::load_graph(u64 n_vertices, u64 n_edges,
                               std::pair<std::vector<u64>, std::vector<u64>>&& edges,
                               std::vector<double>&& weights)
    {
        this->nvert = n_vertices;
        this->nedges = n_edges;
        this->G = std::make_shared<detail::nwgraph>(
            this->nvert, this->nedges, std::move(edges), std::move(weights));
    }
    void pynwgraph::check_loaded() const
    {
        if (this->G.get() == nullptr || this->nvert == 0)
            throw CM_ERROR("Graph is not initialized!!\n");
    }

    // Computation
    void pynwgraph::reset_search()
    {
        this->G->CUR_MAX_CLIQUE_SIZE = 0;
        this->finished_all = false;
        this->current_vertex = 0;
    }

    std::vector<u64> pynwgraph::get_max_clique(double lower_bound, double upper_bound,
                                               bool use_heuristic, bool use_dfs)
    {
        current_vertex = 0;
        check_loaded();
        this->G->CUR_MAX_CLIQUE_SIZE = lower_bound > this->G->CUR_MAX_CLIQUE_SIZE
                                           ? lower_bound
                                           : this->G->CUR_MAX_CLIQUE_SIZE;
        this->G->CLIQUE_LIMIT =
            upper_bound < this->G->CLIQUE_LIMIT ? upper_bound : this->G->CLIQUE_LIMIT;
        this->G->find_max_cliques(current_vertex, use_heuristic, use_dfs);
        finished_all = (current_vertex >= nvert);

        auto ans = this->G->get_max_clique();
        if (lower_bound > this->G->get_clique_weight(ans))
            throw CM_ERROR("Unable to find maximum clique with given bounds (" +
                           std::to_string(this->G->CUR_MAX_CLIQUE_SIZE) + ", " +
                           std::to_string(this->G->CLIQUE_LIMIT) + "]\n");
        return ans;
    }

    std::pair<std::vector<u64>, std::vector<u64>> pynwgraph::get_correspondence2(
        u64 len1, u64 len2, std::vector<u64> clique)
    {
        std::pair<std::vector<u64>, std::vector<u64>> ans;
        u64 i, t1, t2;
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

    std::pair<std::vector<u64>, std::vector<u64>> pynwgraph::get_correspondence(
        u64 len1, u64 len2, double lower_bound, double upper_bound, bool use_heuristic,
        bool use_dfs)
    {
        return this->get_correspondence2(
            len1, len2,
            this->get_max_clique(lower_bound, upper_bound, use_heuristic, use_dfs));
    }
    // IO

    std::string pynwgraph::showdata() const
    {
        std::stringstream ss;
        ss << "cliquematch.core.NWGraph object at " << this << "(";
        ss << "n_vertices=" << this->nvert << ",";
        ss << "n_edges=" << this->nedges << ",";
        ss << ")";
        return ss.str();
    }

    pynwgraph nw_from_edgelist(ndarray<u64> edge_list1, u64 no_of_vertices,
                               ndarray<double> weights1)
    {
        //	std::cout<<"Constructing graph from the list of edges (Nx2 matrix)\n";
        u64 no_of_edges = 0;
        u64 v1, v2;
        auto edge_list = edge_list1.unchecked<2>();
        auto weights = weights1.unchecked<1>();

        std::pair<std::vector<u64>, std::vector<u64>> edges;
        std::vector<double> wts;
        edges.first.resize(no_of_vertices + 1);
        edges.second.resize(no_of_vertices + 1);
        wts.resize(no_of_vertices + 1);

        pynwgraph pg;

        if (static_cast<u64>(weights.shape(0)) != no_of_vertices)
            throw CM_ERROR("vertex weights array does not match number of vertices");

        wts[0] = 0;
        for (u64 i = 0; i < no_of_vertices; i++) wts[i + 1] = weights(i);

        for (u64 i = 0; i < edges.first.size(); i++)
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
        pg.load_graph(no_of_vertices, no_of_edges, std::move(edges), std::move(wts));
        return pg;
    }

    pynwgraph nw_from_adj_matrix(ndarray<bool> adjmat1, ndarray<double> weights1)
    {
        //	std::cout<<"Constructing graph from the adjacency matrix\n";
        auto adjmat = adjmat1.unchecked<2>();
        auto weights = weights1.unchecked<1>();
        if (adjmat.shape(0) != adjmat.shape(1))
            throw CM_ERROR("Adjacency matrix has to be a square matrix!!\n");
        else if (weights.shape(0) != adjmat.shape(0))
            throw CM_ERROR("weights array not as long as matrix rows!!\n");
        else
        {
            u64 no_of_vertices = adjmat.shape(0);
            u64 no_of_edges = 0;
            pynwgraph pg;

            std::pair<std::vector<u64>, std::vector<u64>> edges;
            std::vector<double> wts;

            wts.push_back(0);
            for (u64 i = 0; i < no_of_vertices; i++) wts.push_back(weights(i));

            edges.first.push_back(0);
            edges.second.push_back(0);
            for (u64 i = 0; i < no_of_vertices; i++)
            {
                for (u64 j = 0; j < no_of_vertices; j++)
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
            pg.load_graph(no_of_vertices, no_of_edges, std::move(edges),
                          std::move(wts));
            return pg;
        }
    }
    pynwgraph nw_from_adj_list(u64 n_vertices, u64 n_edges,
                               std::vector<std::set<u64>> edges,
                               std::vector<double> weights)
    {
        u64 e = 0;
        if (edges.size() > 0 && !edges[0].empty())
            throw CM_ERROR("0 is used as sentinel value, set 0 should be empty\n");
        if (n_vertices + 1 != edges.size() || n_vertices != weights.size())
            throw CM_ERROR("Number of vertices don't match!\n");

        std::pair<std::vector<u64>, std::vector<u64>> edges2;
        pynwgraph pg;
        for (u64 i = 0; i < edges.size(); i++)
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
        pg.load_graph(n_vertices, n_edges, std::move(edges2), std::move(weights));
        return pg;
    }

    ndarray<u64> pynwgraph::to_edgelist() const
    {
        check_loaded();
        ndarray<u64> elist1(this->nedges * 2);
        elist1.resize({this->nedges, static_cast<u64>(2)});
        auto elist = elist1.mutable_unchecked<2>();
        u64 k = 0, N = this->nedges;
        this->G->send_data(
            [&elist, &k, &N](u64 i, u64 j)
            {
                // I could use N for checking bounds
                elist(k, 0) = i;
                elist(k, 1) = j;
                k++;
            });
        return elist1;
    }

    ndarray<bool> pynwgraph::to_adj_matrix() const
    {
        check_loaded();
        ndarray<bool> adjmat1(this->nvert * this->nvert);
        adjmat1.resize({this->nvert, this->nvert});
        auto adjmat = adjmat1.mutable_unchecked<2>();
        // zero out while initializing ??
        for (u64 i = 0; i < this->nvert; i++)
            for (u64 j = 0; j < this->nvert; ++j) { adjmat(i, j) = false; }

        // internals are 1-indexed , so subtract by one
        // out of bounds can only happen if either is zero,
        // which means there is already an error elsewhere
        this->G->send_data(
            [&adjmat](u64 i, u64 j)
            {
                adjmat(i - 1, j - 1) = true;
                adjmat(j - 1, i - 1) = true;
            });
        return adjmat1;
    }

    std::vector<std::set<u64>> pynwgraph::to_adj_list() const
    {
        check_loaded();
        std::vector<std::set<u64>> edges(this->nvert + 1);
        this->G->send_data(
            [&edges](u64 i, u64 j)
            {
                edges[i].insert(j);
                edges[j].insert(i);
            });
        return edges;
    }

    std::pair<double, std::set<u64>> pynwgraph::get_vertex_data(u64 i) const
    {
        if (i > this->nvert) throw CM_ERROR("Vertex index out of bounds");
        return this->G->vertex_data(i);
    }

    std::vector<double> pynwgraph::get_all_weights() const
    {
        check_loaded();
        auto ans = this->G->get_all_weights();
        ans.erase(ans.begin());  // remove sentinel
        return ans;
    }

    double pynwgraph::get_clique_weight(std::vector<u64>& clique) const
    {
        check_loaded();
        return this->G->get_clique_weight(clique);
    }

}  // namespace core
}  // namespace cliquematch
