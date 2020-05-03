#include <core/mmio.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

vector<set<size_t> > mmio2_reader(const char* filename, size_t& n_vert, size_t& n_edges)
{
    ifstream f(filename, ios::in);

    size_t dummy;
    if (!f.is_open())
    {
        throw runtime_error("Unable to open Matrix Market File!\n" + string(__FILE__) +
                            "  " + to_string(__LINE__) + "\n");
        vector<set<size_t> > Edges;
        // Edges.data() = NULL;
        return Edges;
    }
    while (f.peek() == '%') f.ignore(2048, '\n');
    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<set<size_t> > Edges(n_vert + 1);
    dummy = 0;

    size_t v1, v2;
    for (size_t i = 0; i < n_edges && !f.eof(); i++)
    {
        f >> v1 >> v2;
        f.ignore(2048, '\n');

        if (v1 > n_vert || v2 > n_vert || v1 == v2)
        {
            dummy++;
            continue;
        }
        Edges[v1].insert(v2);
        Edges[v2].insert(v1);
    }

    f.close();
    if (dummy != 0)
        cerr << "Warning: " << dummy << " invalid edges ignored while reading file\n";
    return Edges;
}
