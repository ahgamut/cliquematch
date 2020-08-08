#ifndef RELSET_H
#define RELSET_H

#include <templates/pair_dist.hpp>
#include <functional>
#include <set>
#include <vector>

namespace cliquematch
{
namespace ext
{
    template <typename List, typename Delta>
    struct relset
    {
        std::size_t N;
        std::vector<pair_dist<Delta> > dists;
        std::function<Delta(List&, std::size_t, std::size_t)> delfunc;
        // delfunc will do the accessing of List,
        // and return the distance value
        bool symmetric;

        relset(std::size_t N,
               std::function<Delta(List&, std::size_t, std::size_t)> dfunc,
               bool symmetric = true);
        relset() { this->N = 0; }
        void fill_dists(List&);
        void disp();
    };
}  // namespace ext
}  // namespace cliquematch
#endif /* RELSET_H */

