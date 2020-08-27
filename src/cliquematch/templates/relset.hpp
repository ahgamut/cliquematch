#ifndef RELSET_HPP
#define RELSET_HPP

#include <templates/relset.h>
#include <algorithm>

namespace cliquematch
{
namespace ext
{
    // template syntax brain hurty
    template <typename List, typename Delta>
    relset<List, Delta>::relset(
        std::size_t N,
        const std::function<Delta(const List&, const std::size_t, const std::size_t)>&
            dfunc,
        bool symmetric)
        : symmetric(symmetric), N(N), delfunc(dfunc)
    {
        if (this->symmetric)
            this->dists.resize(N * (N - 1) / 2);
        else
            this->dists.resize(N * (N - 1));
    }

    template <typename List, typename Delta>
    void relset<List, Delta>::fill_dists(const List& x)
    {
        std::size_t i, j, count = 0;
        for (i = 0; i < this->N; i++)
        {
            for (j = (this->symmetric ? i + 1 : 0); j < this->N; j++)
            {
                if (j == i) continue;
                this->dists[count].first = i;
                this->dists[count].second = j;
                this->dists[count].dist = this->delfunc(x, i, j);
                count++;
            }
        }
        std::sort(this->dists.begin(), this->dists.end());
    }

    /* needed only for debugging
    template <typename List, typename Delta>
    void relset<List, Delta>::disp() const
    {
        for (std::size_t i = 0; i < this->dists.size(); i++)
        {
            std::cout << this->dists[i].first << " " << this->dists[i].second << " "
                      << this->dists[i].dist << "\n";
        }
    }
    */
}  // namespace ext
}  // namespace cliquematch
#endif /* RELSET_HPP */

