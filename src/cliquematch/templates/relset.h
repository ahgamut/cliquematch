#ifndef RELSET_H
#define RELSET_H

#include <templates/pair_dist.hpp>
#include <functional>
#include <vector>

namespace cliquematch
{
namespace ext
{
    template <typename List, typename Delta>
    struct relset
    {
        const bool symmetric;
        const std::size_t N;
        const std::function<Delta(const List&, const std::size_t, const std::size_t)>&
            delfunc;
        // delfunc will do the accessing of List,
        // and return the distance value
        std::vector<pair_dist<Delta> > dists;

        relset(std::size_t N,
               const std::function<Delta(const List&, const std::size_t,
                                         const std::size_t)>& dfunc,
               bool symmetric = true);
        relset& operator=(relset&) = delete;
        relset(const relset&) = delete;
        relset(relset&&) = delete;
        void fill_dists(const List&);
        // void disp() const; needed only for debugging
    };
}  // namespace ext
}  // namespace cliquematch
#endif /* RELSET_H */

