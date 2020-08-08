#ifndef PAIR_DIST_HPP
#define PAIR_DIST_HPP

#include <templates/pair_dist.h>
#include <iostream>

namespace cliquematch
{
namespace ext
{
    template <class T>
    void pair_dist<T>::disp()
    {
        std::cerr << this->first << " " << this->second << " " << this->dist << "\n";
    }
}  // namespace ext
}  // namespace cliquematch
#endif /* PAIR_DIST_HPP */

