#include <core/mmio.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

vector<set<std::size_t> > mmio2_reader(const char* filename, std::size_t& n_vert,
                               std::size_t& n_edges) {
    ifstream f(filename, ios::in);

    std::size_t dummy;
    if (!f.is_open()) {
        throw runtime_error(
            "Unable to open file of form MMIO2 (vertex-vertex)!\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        vector<set<std::size_t> > Edges;
        // Edges.data() = NULL;
        return Edges;
    }
    while (f.peek() == '%') f.ignore(2048, '\n');
    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<set<std::size_t> > Edges(n_vert + 1);

    std::size_t v1, v2;
    for (std::size_t i = 0; i < n_edges && !f.eof(); i++) {
        f >> v1 >> v2;
        if (v1 == v2) continue;
        Edges[v1].insert(v2);
        Edges[v2].insert(v1);
    }
    f.close();

    return Edges;
}

vector<set<std::size_t> > mmio3_reader(const char* filename, std::size_t& n_vert,
                               std::size_t& n_edges) {
    ifstream f(filename, ios::in);

    double time_taken;

    std::size_t dummy;
    if (!f.is_open()) {
        throw runtime_error(
            "Unable to open file of form MMIO3: "
            "(vertex-vertex-dummyweight)!\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        vector<set<std::size_t> > Edges;
        // Edges.data() = NULL;
        return Edges;
    }

    while (f.peek() == '%') f.ignore(2048, '\n');
    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<set<std::size_t> > Edges(n_vert + 1);

    std::size_t v1, v2;
    for (std::size_t i = 0; i < n_edges && !f.eof(); i++) {
        f >> v1 >> v2 >> time_taken;
        if (v1 == v2) continue;
        Edges[v1].insert(v2);
        Edges[v2].insert(v1);
    }
    f.close();

    return Edges;
}
