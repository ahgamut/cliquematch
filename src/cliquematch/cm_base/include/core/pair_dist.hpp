#ifndef PAIR_DIST_HPP
#define PAIR_DIST_HPP

#include <cm_base/include/core/pair_dist.h>
#include <iostream>

template <class T>
void pair_dist<T>::disp() {
    std::cerr << this->first << " " << this->second << " " << this->dist
              << "\n";
}

#endif /* PAIR_DIST_HPP */

