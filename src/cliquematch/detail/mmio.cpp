#include <detail/mmio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

namespace cliquematch
{
namespace detail
{
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> mmio4_reader(
        const char* filename, std::size_t& n_vert, std::size_t& n_edges)
    {
        using namespace std;
        ifstream f(filename, ios::in);
        if (!f.is_open())
        {
            throw runtime_error("Unable to open Matrix Market File!\n" +
                                string(__FILE__) + "  " + to_string(__LINE__) + "\n");
        }
        // the Matrix Market format has a preamble:
        // a number of lines starting with % or #
        // describing the origin of the graph, who made it,
        // and some properties
        while (f.peek() == '%' || f.peek() == '#') f.ignore(2048, '\n');

        std::size_t n_loops = 0, n_invalids = 0, dummy, i, j;
        std::size_t v1, v2;

        // the file then has the number of vertices twice
        // followed by the number of edges
        f >> n_vert;
        f >> dummy;
        f >> n_edges;

        pair<vector<std::size_t>, vector<std::size_t>> Edges;
        // each edge is going to stored twice
        // plus dummy loop edges for easier cliques
        // plus a sentinel vertex to show errors
        Edges.first.resize(2 * n_edges + n_vert + 1);
        Edges.second.resize(2 * n_edges + n_vert + 1);

        // fill the loop edges first.  A better way might be to fill these at
        // the end, and filling only those vertices which have neighbors.
        for (j = 0; j < n_vert + 1; j++)
        {
            Edges.first[j] = j;
            Edges.second[j] = j;
        }
        for (i = 0, j = n_vert + 1; i < n_edges && !f.eof(); i++, j += 2)
        {
            f >> v1 >> v2;
            // there might be some additional data, like edge weights
            f.ignore(2048, '\n');

            // ignore any invalid edges or loops
            if (v1 > n_vert || v2 > n_vert || v1 == v2)
            {
                (v1 != v2) ? n_invalids++ : n_loops++;
                Edges.first[j] = 0;
                Edges.second[j] = 0;
                Edges.first[j + 1] = 0;
                Edges.second[j + 1] = 0;
            }
            else
            {
                Edges.first[j] = v1;
                Edges.second[j] = v2;
                Edges.first[j + 1] = v2;
                Edges.second[j + 1] = v1;
            }
        }
        f.close();

        if (n_invalids != 0)
            cerr << "Warning: " << n_invalids
                 << " invalid edges ignored while reading file\n";
        return Edges;
    }
}  // namespace detail
}  // namespace cliquematch
