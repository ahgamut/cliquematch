#include <ext/Isograph.h>

namespace py = pybind11;

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
    class_<Isograph, std::unique_ptr<Isograph, pygraphDeleter>, pygraph>(
        m, "IsoGraph",
        "specialized template subclass for subgraph isomorphism via correspondence "
        "graph")
        .def(init<>())
        .def("_build_edges", &Isograph::build_edges, "g1"_a, "g2"_a)
        .def("_get_correspondence", &Isograph::get_correspondence)
        .def("__repr__", [](Isograph& zz) { return zz.showdata(); })
        .def("__str__", [](Isograph& zz) { return zz.showdata(); });
}
