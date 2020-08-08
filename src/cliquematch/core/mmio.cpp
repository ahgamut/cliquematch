#include <core/mmio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

namespace cliquematch
{
namespace detail
{
    std::vector<std::pair<std::size_t, std::size_t>> mmio3_reader(const char* filename,
                                                                  std::size_t& n_vert,
                                                                  std::size_t& n_edges)
    {
        using namespace std;
        ifstream f(filename, ios::in);
        if (!f.is_open())
        {
            throw runtime_error("Unable to open Matrix Market File!\n" +
                                string(__FILE__) + "  " + to_string(__LINE__) + "\n");
        }
        while (f.peek() == '%' || f.peek() == '#') f.ignore(2048, '\n');

        std::size_t n_loops = 0, n_invalids = 0, dummy, i, j;
        std::size_t v1, v2;

        f >> n_vert;
        f >> dummy;
        f >> n_edges;

        vector<pair<std::size_t, std::size_t>> Edges(2 * n_edges + n_vert + 1);

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
            else
            {
                Edges[j] = {v1, v2};
                Edges[j + 1] = {v2, v1};
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
