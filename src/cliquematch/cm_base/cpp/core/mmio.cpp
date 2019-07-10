#include <cm_base/include/core/mmio.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

vector<set<u32> > mmio2_reader(const char* filename, u32& n_vert,
                               u32& n_edges) {
    ifstream f(filename, ios::in);

    u32 dummy;
    if (!f.is_open()) {
        throw runtime_error(
            "Unable to open file of form MMIO2 (vertex-vertex)!\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        vector<set<u32> > Edges;
        // Edges.data() = NULL;
        return Edges;
    }
    while (f.peek() == '%') f.ignore(2048, '\n');
    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<set<u32> > Edges(n_vert + 1);

    u32 v1, v2;
    for (u32 i = 0; i < n_edges && !f.eof(); i++) {
        f >> v1 >> v2;
        if (v1 == v2) continue;
        Edges[v1].insert(v2);
        Edges[v2].insert(v1);
    }
    f.close();

    return Edges;
}

vector<set<u32> > mmio3_reader(const char* filename, u32& n_vert,
                               u32& n_edges) {
    ifstream f(filename, ios::in);

    double time_taken;

    u32 dummy;
    if (!f.is_open()) {
        throw runtime_error(
            "Unable to open file of form MMIO3: "
            "(vertex-vertex-dummyweight)!\n" +
            std::string(__FILE__) + "  " + std::to_string(__LINE__) + "\n");
        vector<set<u32> > Edges;
        // Edges.data() = NULL;
        return Edges;
    }

    while (f.peek() == '%') f.ignore(2048, '\n');
    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<set<u32> > Edges(n_vert + 1);

    u32 v1, v2;
    for (u32 i = 0; i < n_edges && !f.eof(); i++) {
        f >> v1 >> v2 >> time_taken;
        if (v1 == v2) continue;
        Edges[v1].insert(v2);
        Edges[v2].insert(v1);
    }
    f.close();

    return Edges;
}
