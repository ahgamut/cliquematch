#include <cm_base/include/core/triples.h>
#include <iostream>

template <class T>
void triple<T>::disp() {
    std::cerr << this->first << " " << this->second << " " << this->dist
              << "\n";
}

template struct triple<double>;
template short binary_find2<double>(dist_2d*, u32, double, u32&);
