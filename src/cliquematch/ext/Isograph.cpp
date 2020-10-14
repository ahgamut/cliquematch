#include <ext/Isograph.h>

namespace cliquematch
{
namespace ext
{
    bool build_edges_for_iso(pygraph& pg, const pygraph& s1, const pygraph& s2)
    {
        std::size_t no_of_vertices, no_of_edges;
        if (s1.nvert == 0 || s2.nvert == 0)
            throw CM_ERROR("One of the inputs is an empty graph");

        auto edges = iso_edges(no_of_vertices, no_of_edges, s1, s2);

        if (edges.first.size() == 0 || edges.second.size() == 0)
            throw CM_ERROR("Could not extract edges");
        pg.load_graph(no_of_vertices, no_of_edges, std::move(edges));

        return true;
    }
}  // namespace ext
void init_Isograph(pybind11::module& mm)
{
    using namespace pybind11;
    mm.def("_build_edges", &ext::build_edges_for_iso, "G"_a.none(false),
           "g1"_a.none(false), "g2"_a.none(false));
}
}  // namespace cliquematch
