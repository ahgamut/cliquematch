#include <core/pyiterator.h>
#include <pybind11/stl.h>

namespace cliquematch
{
namespace core
{
    std::vector<std::size_t> CliqueIterator::next_clique()
    {
        std::size_t v;
        while (true)
        {
            v = en->process_graph(*(pg.G));
            if (v > pg.nvert) break;
            return pg.G->get_max_clique(v);
        }
        throw pybind11::stop_iteration();
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
    CorrespondenceIterator::next_clique()
    {
        std::size_t v;
        while (true)
        {
            v = en->process_graph(*(pg.G));
            if (v > pg.nvert) break;
            return pg.get_correspondence2(len1, len2, pg.G->get_max_clique(v));
        }
        throw pybind11::stop_iteration();
    }
}  // namespace core

void init_iterators(pybind11::module& m)
{
    using namespace pybind11;
    namespace py = pybind11;
    class_<core::CliqueIterator>(m, "CliqueIterator")
        .def("__iter__", [](core::CliqueIterator& it) { return it; })
        .def("__next__", &core::CliqueIterator::next_clique);
    class_<core::CorrespondenceIterator>(m, "CorrespondenceIterator")
        .def("__iter__", [](core::CorrespondenceIterator& it) { return it; })
        .def("__next__", &core::CorrespondenceIterator::next_clique);
}
}  // namespace cliquematch
