#include <core/mmio.h>
#include <core/pygraph.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;

pygraph::pygraph()
{
    nvert = 0;
    nedges = 0;
    use_heur = false;
    use_dfs = true;
    lower_bound = 1;
    upper_bound = 32;
    time_lim = 100.0;
    current_vertex = 0;
    finished_heur = false;
    finished_all = false;
    ans_found = false;
    std::vector<std::set<std::size_t>>().swap(this->EDGES);
}

pygraph::pygraph(std::size_t n_vertices, std::size_t n_edges,
                 std::vector<std::set<std::size_t>> edges)
    : pygraph()
{
    this->load_graph(n_vertices, n_edges, edges);
}

// Separate method to make exporting pygraph easier
void pygraph::load_graph(std::size_t n_vertices, std::size_t n_edges,
                         std::vector<std::set<std::size_t>> edges)
{
    this->nvert = n_vertices;
    this->nedges = n_edges;
    std::vector<std::set<std::size_t>>().swap(this->EDGES);
    this->EDGES = edges;
    this->G = graph(this->nvert, this->nedges, EDGES);
}

//' Helper function to find maximum clique, does not return anything
void pygraph::find_max_clique()
{
    if (G.n_vert == 0)
    {
        throw std::runtime_error("Graph is not initialized!!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    //	std::cerr<<"Finding cliques\n";
    this->G.CUR_MAX_CLIQUE_SIZE = this->lower_bound > this->G.CUR_MAX_CLIQUE_SIZE
                                      ? this->lower_bound
                                      : this->G.CUR_MAX_CLIQUE_SIZE;
    this->G.CLIQUE_LIMIT = this->upper_bound;
    G.find_max_cliques(current_vertex, finished_heur, use_heur, use_dfs, time_lim);
    ans_clique = G.get_max_clique();
    ans_found = true;
    finished_all = finished_heur && (current_vertex >= nvert);
}

// Finds the maximum clique and returns it as a std::vector
std::vector<std::size_t> pygraph::get_max_clique()
{
    if (!ans_found) find_max_clique();
    if (this->lower_bound > this->ans_clique.size() || this->G.CUR_MAX_CLIQUE_LOC == 0)
    {
        throw std::runtime_error("Unable to find maximum clique with given bounds\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }
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
    this->G.CUR_MAX_CLIQUE_SIZE = 1;
    this->G.CUR_MAX_CLIQUE_LOC = 0;
}

std::string pygraph::showdata()
{
    std::stringstream ss;
    ss << "cliquematch.core.Graph object at " << this << "\n(";
    ss << "n_vertices=" << this->nvert << ",";
    ss << "n_edges=" << this->nedges << ",";
    ss << "lower_bound=" << this->lower_bound << ",";
    ss << "upper_bound=" << this->upper_bound << ",";
    ss << "time_limit=" << this->time_lim << ",";
    ss << "use_heuristic=" << (this->use_heur ? "True" : "False") << ",";
    ss << "use_dfs=" << (this->use_dfs ? "True" : "False") << ",";
    ss << "search_done=" << (this->finished_all ? "True" : "False");
    ss << ")";
    return ss.str();
}

pygraph from_file(std::string filename, bool weighted)
{
    //	std::cout<<"Constructing graph from a file\n";
    std::string fname = filename;
    std::cerr << "Loading graph from: " << fname << "\n";

    std::vector<std::set<std::size_t>> edges;
    std::size_t nvert, nedges;

    if (!weighted)
    {
        std::cerr << "Choice: 1 MMIO format -> line: edge edge\n";
        edges = mmio2_reader(fname.c_str(), nvert, nedges);
    }
    else
    {
        std::cerr << "Choice: 2 MMIO format -> line: edge edge weight\n";
        std::cerr << "Note that weight of graph is not recorded\n";
        edges = mmio3_reader(fname.c_str(), nvert, nedges);
    }

    if (edges.data() == NULL || edges.size() == 0)
    {
        throw std::runtime_error("Could not extract edges!!\n" + std::string(__FILE__) +
                                 "  " + std::to_string(__LINE__) + "\n");
    }

    return pygraph(nvert, nedges, edges);
}

pygraph from_edgelist(ndarray<std::size_t> edge_list1, std::size_t no_of_vertices)
{
    //	std::cout<<"Constructing graph from the list of edges (Nx2 matrix)\n";
    std::size_t no_of_edges = 0;
    std::size_t v1, v2;
    std::vector<std::set<std::size_t>> edges(no_of_vertices + 1);
    auto edge_list = edge_list1.unchecked<2>();

    for (auto i = 0; i < edge_list.shape(0); i++)
    {
        v1 = edge_list(i, 0);
        v2 = edge_list(i, 1);
        if (v1 > no_of_vertices || v2 > no_of_vertices)
        {
            throw std::runtime_error(
                "Edge contains vertex ID > number of vertices specified\n" +
                std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        }
        if (v1 == 0 || v2 == 0)
        {
            throw std::runtime_error(
                "Vertex numbers must begin at 1, 0 is used as a sentinel "
                "value\n" +
                std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        }
        edges[v1].insert(v2);
        edges[v2].insert(v1);
    }

    if (edges.data() == NULL || edges.size() == 0)
    {
        throw std::runtime_error("Could not extract edges!!\n" + std::string(__FILE__) +
                                 "  " + std::to_string(__LINE__) + "\n");
    }

    for (std::size_t i = 0; i < edges.size(); i++) { no_of_edges += edges[i].size(); }
    no_of_edges /= 2;
    return pygraph(no_of_vertices, no_of_edges, edges);
}

pygraph from_adj_matrix(ndarray<bool> adjmat1)
{
    //	std::cout<<"Constructing graph from the adjacency matrix\n";
    auto adjmat = adjmat1.unchecked<2>();
    if (adjmat.shape(0) != adjmat.shape(1))
    {
        throw std::runtime_error("Adjacency matrix has to be a square matrix!!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }
    else
    {
        std::size_t no_of_vertices = adjmat.shape(0);
        std::size_t no_of_edges = 0;

        std::vector<std::set<std::size_t>> edges(no_of_vertices + 1);

        for (std::size_t i = 0; i < no_of_vertices; i++)
        {
            for (std::size_t j = 0; j < no_of_vertices; j++)
            {
                if (adjmat(i, j) && i != j)
                {
                    edges[i + 1].insert(j + 1);
                    edges[j + 1].insert(i + 1);
                    no_of_edges++;
                }
            }
        }

        no_of_edges /= 2;
        if (edges.data() == NULL || edges.size() == 0)
        {
            throw std::runtime_error("Could not extract edges!!\n" +
                                     std::string(__FILE__) + "  " +
                                     std::to_string(__LINE__) + "\n");
        }

        return pygraph(no_of_vertices, no_of_edges, edges);
    }
}

