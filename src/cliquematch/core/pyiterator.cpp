#include <core/pyiterator.h>
#include <pybind11/stl.h>

namespace cliquematch
{
namespace core
{
    std::vector<u64> CliqueIterator::next_clique()
    {
        u64 v;
        while (true)
        {
            v = en->process_graph(*(pg.G));
            if (v > pg.nvert) break;
            return pg.G->get_max_clique(v);
        }
        throw pybind11::stop_iteration();
    }

    std::pair<std::vector<u64>, std::vector<u64>> CorrespondenceIterator::next_clique()
    {
        u64 v;
        while (true)
        {
            v = en->process_graph(*(pg.G));
            if (v > pg.nvert) break;
            return pg.get_correspondence2(len1, len2, pg.G->get_max_clique(v));
        }
        throw pybind11::stop_iteration();
    }

    std::vector<u64> NWCliqueIterator::next_clique()
    {
        u64 v;
        while (true)
        {
            v = en->process_graph(*(pg.G));
            if (v > pg.nvert) break;
            return pg.G->get_max_clique(v);
        }
        throw pybind11::stop_iteration();
    }

    std::pair<std::vector<u64>, std::vector<u64>>
    NWCorrespondenceIterator::next_clique()
    {
        u64 v;
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
    class_<core::NWCliqueIterator>(m, "NWCliqueIterator")
        .def("__iter__", [](core::NWCliqueIterator& it) { return it; })
        .def("__next__", &core::NWCliqueIterator::next_clique);
    class_<core::NWCorrespondenceIterator>(m, "NWCorrespondenceIterator")
        .def("__iter__", [](core::NWCorrespondenceIterator& it) { return it; })
        .def("__next__", &core::NWCorrespondenceIterator::next_clique);
}
}  // namespace cliquematch
