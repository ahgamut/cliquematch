#include <cm_base/wrappy/graph2/graph2.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;

//' Default constructor, use only for debugging purposes
graph2::graph2() {
    nvert = 0;
    nedges = 0;
    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100.0;
    current_vertex = 0;
    finished_heur = false;
    finished_all = false;
    std::vector<std::set<u32>>().swap(this->EDGES);
}

//' Construct using a file read in (a particular) Matrix Market format
//' Supports only a specific type of .mtx file
//' For `reader_choice` == 1 file should be of the form
//'
//' <no of vertices>
//' <no of edges>
//' <v1> <v2>
//' ..
//' ..
//' ..
//' and so on.
//' For `reader_choice` == 2 file should be of the form
//'
//' <no of vertices>
//' <dummy value>
//' <no of edges>
//' <v1> <v2> <weight>
//' ..
//' ..
//' ..
//' and so on.
//' Note that weight will not be considered in clique computation (not even
// recorded anywhere)
graph2::graph2(std::string filename, unsigned int reader_choice) {
    //	std::cout<<"Constructing graph from a file\n";
    std::string fname = filename;
    std::cerr << "Loading graph from: " << fname << "\n";

    std::vector<std::set<u32>>().swap(this->EDGES);
    if (reader_choice == 1) {
        std::cerr << "Choice: 1 MMIO format -> line: edge edge\n";
        EDGES = mmio2_reader(fname.c_str(), nvert, nedges);
    } else if (reader_choice == 2) {
        std::cerr << "Choice: 2 MMIO format -> line: edge edge weight\n";
        std::cerr << "Note that weight of graph is not recorded\n";
        EDGES = mmio3_reader(fname.c_str(), nvert, nedges);
    } else {
        throw std::runtime_error("Invalid reading option given!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    if (EDGES.data() == NULL || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges!!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    this->G = graph(nvert, nedges, EDGES);

    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100.0;
    current_vertex = 0;
    finished_heur = false;
    finished_all = false;
}

//' Graph from adjacency list
//' Vertices must be numbered 1 to N
//' No self loops, no weights, only undirected
graph2::graph2(ndarray<unsigned int> edge_list1, unsigned int no_of_vertices) {
    //	std::cout<<"Constructing graph from the list of edges (Nx2 matrix)\n";
    nvert = no_of_vertices;
    u32 no_of_edges = 0;

    std::vector<std::set<u32>>().swap(this->EDGES);
    EDGES = std::vector<std::set<u32>>(nvert + 1);

    u32 v1, v2;

    auto edge_list = edge_list1.unchecked<2>();
    for (u32 i = 0; i < edge_list.shape(0); i++) {
        v1 = edge_list(i, 0);
        v2 = edge_list(i, 1);
        if (v1 > nvert || v2 > nvert) {
            throw std::runtime_error(
                "Edge contains vertex ID > number of vertices specified\n" +
                std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        }
        if (v1 == 0 || v2 == 0) {
            throw std::runtime_error(
                "Vertex numbers must begin at 1, 0 is used as a sentinel "
                "value\n" +
                std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        }
        EDGES[v1].insert(v2);
        EDGES[v2].insert(v1);
    }

    if (EDGES.data() == NULL || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges!!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    for (u32 i = 0; i < this->EDGES.size(); i++) {
        no_of_edges += this->EDGES[i].size();
    }
    no_of_edges /= 2;

    nvert = no_of_vertices;
    nedges = no_of_edges;

    //	std::cerr<<"Extracted edges successfully, initializing graph\n";
    this->G = graph(nvert, nedges, EDGES);
    // std::cerr << "Graph initialized successfully:: " << G.n_vert << " " <<
    // G.el_size << "\n";

    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100.0;
    current_vertex = 0;
    finished_heur = false;
    finished_all = false;
}

//' Graph from adjacency matrix (square matrix of booleans)
//' Vertices assumed numbered 1 to N
//' No self loops, no weights, only undirected
graph2::graph2(ndarray<bool> adjmat1) {
    //	std::cout<<"Constructing graph from the adjacency matrix\n";
    std::vector<std::set<u32>>().swap(this->EDGES);
    auto adjmat = adjmat1.unchecked<2>();
    if (adjmat.shape(0) != adjmat.shape(1)) {
        throw std::runtime_error(
            "Adjacency matrix has to be a square matrix!!\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
    } else {
        nvert = adjmat.shape(0);
        u32 no_of_edges = 0;

        EDGES = std::vector<std::set<u32>>(nvert + 1);

        for (u32 i = 0; i < nvert; i++) {
            for (u32 j = 0; j < nvert; j++) {
                if (adjmat(i, j) && i != j) {
                    EDGES[i + 1].insert(j + 1);
                    EDGES[j + 1].insert(i + 1);
                    no_of_edges++;
                }
            }
        }

        no_of_edges /= 2;
        if (EDGES.data() == NULL || EDGES.size() == 0) {
            throw std::runtime_error("Could not extract edges!!\n" +
                                     std::string(__FILE__) + "  " +
                                     std::to_string(__LINE__) + "\n");
        }

        nedges = no_of_edges;

        //		std::cerr<<"Extracted edges successfully, initializing
        // graph\n";
        this->G = graph(nvert, nedges, EDGES);
        //		std::cerr<<"Graph initialized successfully::
        //"<<G.n_vert<<" "<<G.el_size<<"\n";

        use_heur = false;
        use_dfs = true;
        clique_lim = 32;
        time_lim = 100.0;
        current_vertex = 0;
        finished_heur = false;
        finished_all = false;
    }
}

//' Helper function to find maximum clique, does not return anything
void graph2::find_max_clique() {
    G.CLIQUE_LIMIT = clique_lim;
    if (G.n_vert == 0) {
        throw std::runtime_error("Graph is not initialized!!\n" +
                                 std::string(__FILE__) + "  " +
                                 std::to_string(__LINE__) + "\n");
    }

    //	std::cerr<<"Finding cliques\n";
    G.find_max_cliques(current_vertex, finished_heur, use_heur, use_dfs,
                       time_lim);
    ans_clique = G.get_max_clique();
    ans_found = true;
    finished_all = finished_heur && (current_vertex >= nvert);
    if (finished_all) std::cerr << "Search complete\n";
}

// Finds the maximum clique and returns it as a std::vector
std::vector<u32> graph2::get_max_clique() {
    if (!ans_found) find_max_clique();
    return this->ans_clique;
}

//' The clique search can be reset in case the current set of parameters were
// not optimal
void graph2::continue_search() {
    if (!finished_all) {
        ans_found = false;
        find_max_clique();
    } else {
        std::cerr << "Search is complete.\n";
    }
}

std::string graph2::showdata() {
    std::stringstream ss;
    ss << "cliquematch.cm_base.graph2 object at " << this << "\n";
    ss << this->nvert << " vertices, " << this->nedges << " edges\n";
    ss << "Search Parameters : \n";
    ss << "Size Limit: " << this->clique_lim
       << " Time limit: " << this->time_lim << "s\n";
    ss << "Using Heuristic : " << (this->use_heur ? "True" : "False");
    ss << " Using DFS: " << (this->use_dfs ? "True" : "False");
    ss << "\n";
    ss << "Search Completed? : " << (this->finished_all ? "True" : "False")
       << "\n";
    return ss.str();
}

