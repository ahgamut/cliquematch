#include <core/mmio.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

vector<pair<size_t, size_t>> mmio3_reader(const char* filename, size_t& n_vert,
                                          size_t& n_edges)
{
    ifstream f(filename, ios::in);
    if (!f.is_open())
    {
        throw runtime_error("Unable to open Matrix Market File!\n" + string(__FILE__) +
                            "  " + to_string(__LINE__) + "\n");
    }
    while (f.peek() == '%' || f.peek() == '#') f.ignore(2048, '\n');

    size_t n_loops = 0, n_invalids = 0, dummy, i, j;
    size_t v1, v2;

    f >> n_vert;
    f >> dummy;
    f >> n_edges;

    vector<pair<size_t, size_t>> Edges(2 * n_edges + n_vert + 1);

    for (j = 0; j < n_vert + 1; j++) Edges[j] = {j, j};
    for (i = 0, j = n_vert + 1; i < n_edges && !f.eof(); i++, j += 2)
    {
        f >> v1 >> v2;
        f.ignore(2048, '\n');

        if (v1 > n_vert || v2 > n_vert || v1 == v2)
        {
            (v1 != v2) ? n_invalids++ : n_loops++;
            Edges[j] = {0, 0};
            Edges[j + 1] = {0, 0};
        }
        Edges[j] = {v1, v2};
        Edges[j + 1] = {v2, v1};
    }
    f.close();

    std::sort(Edges.begin(), Edges.end());
    auto it = std::unique(Edges.begin(), Edges.end());
    Edges.resize(std::distance(Edges.begin(), it));
    n_edges = (Edges.size() - (n_vert + 1)) / 2;

    if (n_invalids != 0)
        cerr << "Warning: " << n_invalids
             << " invalid edges ignored while reading file\n";
    return Edges;
}
