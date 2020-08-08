#ifndef RELSET_HPP
#define RELSET_HPP

#include <templates/relset.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>

namespace cliquematch
{
namespace ext
{
    // template syntax brain hurty
    template <typename List, typename Delta>
    relset<List, Delta>::relset(
        std::size_t N, std::function<Delta(List&, std::size_t, std::size_t)> dfunc,
        bool symmetric)
    {
        this->N = N;
        this->delfunc = dfunc;
        this->symmetric = symmetric;
        if (symmetric)
            this->dists = std::vector<pair_dist<Delta> >(N * (N - 1) / 2);
        else
            this->dists = std::vector<pair_dist<Delta> >(N * (N - 1));
    }

    template <typename List, typename Delta>
    void relset<List, Delta>::fill_dists(List& x)
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

    template <typename List, typename Delta>
    void relset<List, Delta>::disp()
    {
        for (std::size_t i = 0; i < this->dists.size(); i++)
        {
            std::cout << this->dists[i].first << " " << this->dists[i].second << " "
                      << this->dists[i].dist << "\n";
        }
    }
}  // namespace ext
}  // namespace cliquematch
#endif /* RELSET_HPP */

