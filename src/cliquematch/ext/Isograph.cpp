#include <ext/Isograph.h>

namespace py = pybind11;

std::vector<std::set<std::size_t>> iso_edges(std::size_t& num_vertices,
                                             std::size_t& num_edges, const graph& g1,
                                             const graph& g2)
{
    num_vertices = (g1.n_vert - 1) * (g2.n_vert - 1);
    num_edges = 0;
    std::vector<std::set<std::size_t>> edges(num_vertices + 1);

    std::size_t i1, i2, j1, j2, v1, v2;
    std::size_t k, l;
    short f1, f2;

    for (i1 = 1; i1 < g1.n_vert; ++i1)
    {
        for (i2 = i1 + 1; i2 < g1.n_vert; i2++)
        {
            f1 = g1.find_if_neighbors(g1.vertices[i1], i2, k);
            for (j1 = 1; j1 < g2.n_vert; ++j1)
            {
                for (j2 = j1 + 1; j2 < g2.n_vert; ++j2)
                {
                    f2 = g2.find_if_neighbors(g2.vertices[j1], j2, l);
                    if ((f1 != 1) && (f2 == 1)) continue;

                    v1 = (i1 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                    v2 = (i2 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                    edges[v1].insert(v2);
                    edges[v2].insert(v1);

                    v1 = (i2 - 1) * (g2.n_vert - 1) + (j1 - 1) + 1;
                    v2 = (i1 - 1) * (g2.n_vert - 1) + (j2 - 1) + 1;
                    edges[v1].insert(v2);
                    edges[v2].insert(v1);

                    num_edges += 2;
                }
            }
        }
    }
    return edges;
}

std::vector<std::set<std::size_t>> iso_edges(std::size_t& nv, std::size_t& ne,
                                             const pygraph& g1, const pygraph& g2)
{
    return iso_edges(nv, ne, g1.G, g2.G);
}

bool Isograph::build_edges(pygraph& s1, pygraph& s2)
{
    std::size_t no_of_vertices, no_of_edges;
    if (s1.nvert == 0 || s2.nvert == 0)
        throw CM_ERROR("One of the inputs is an empty graph");

    auto edges = iso_edges(no_of_vertices, no_of_edges, s1, s2);

    if (edges.data() == nullptr || edges.size() == 0)
        throw CM_ERROR("Could not extract edges");
    this->load_graph(no_of_vertices, no_of_edges, edges);

    return true;
}

void init_Isograph(py::module& m)
{
    using namespace pybind11;
    class_<Isograph, pygraph>(m, "IsoGraph")
        .def(init<>())
        .def("build_edges", &Isograph::build_edges, "g1"_a, "g2"_a)
        .def("get_correspondence", &Isograph::get_correspondence)
        .def("__repr__", [](Isograph& zz) { return zz.showdata(); })
        .def("__str__", [](Isograph& zz) { return zz.showdata(); });
}
