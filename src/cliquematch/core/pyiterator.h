#ifndef PYITERATOR_H
#define PYITERATOR_H

#include <core/pygraph.h>
#include <core/pynwgraph.h>
#include <detail/graph/dfs.h>
#include <detail/nwgraph/nwclique.h>

namespace cliquematch
{
namespace core
{
    class CliqueIterator
    {
       private:
        pygraph& pg;
        pybind11::object rf;
        std::shared_ptr<detail::CliqueEnumerator> en;

       public:
        CliqueIterator(pygraph& p, pybind11::object rf, u64 clique_size)
            : pg(p), rf(rf), en(new detail::CliqueEnumerator(clique_size))
        {
            if (pg.G.get() == nullptr || pg.nvert == 0)
                throw CM_ERROR("trying to create iterator on uninitialized Graph");
        };
        std::vector<u64> next_clique();
    };

    class CorrespondenceIterator
    {
       private:
        pygraph& pg;
        pybind11::object rf;
        std::shared_ptr<detail::CliqueEnumerator> en;
        u64 len1, len2;

       public:
        CorrespondenceIterator(pygraph& p, pybind11::object rf, u64 len1, u64 len2,
                               u64 clique_size)
            : pg(p), rf(rf), en(new detail::CliqueEnumerator(clique_size)), len1(len1),
              len2(len2)
        {
            if (pg.G.get() == nullptr || pg.nvert == 0)
                throw CM_ERROR("trying to create iterator on uninitialized Graph");
        };
        std::pair<std::vector<u64>, std::vector<u64>> next_clique();
    };

    class NWCliqueIterator
    {
       private:
        pynwgraph& pg;
        pybind11::object rf;
        std::shared_ptr<detail::NWCliqueEnumerator> en;

       public:
        NWCliqueIterator(pynwgraph& p, pybind11::object rf, double clique_size)
            : pg(p), rf(rf), en(new detail::NWCliqueEnumerator(clique_size))
        {
            if (pg.G.get() == nullptr || pg.nvert == 0)
                throw CM_ERROR("trying to create iterator on uninitialized Graph");
        };
        std::vector<u64> next_clique();
    };

    class NWCorrespondenceIterator
    {
       private:
        pynwgraph& pg;
        pybind11::object rf;
        std::shared_ptr<detail::NWCliqueEnumerator> en;
        u64 len1, len2;

       public:
        NWCorrespondenceIterator(pynwgraph& p, pybind11::object rf, u64 len1, u64 len2,
                                 double clique_size)
            : pg(p), rf(rf), en(new detail::NWCliqueEnumerator(clique_size)),
              len1(len1), len2(len2)
        {
            if (pg.G.get() == nullptr || pg.nvert == 0)
                throw CM_ERROR("trying to create iterator on uninitialized Graph");
        };
        std::pair<std::vector<u64>, std::vector<u64>> next_clique();
    };

}  // namespace core
}  // namespace cliquematch
#endif /* PYITERATOR_H */
