#ifndef CORE_H
#define CORE_H

#include <set>
#include <vector>
#include <utility>
#include <cstdint>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <pybind11/pybind11.h>
// need to #include<cmath> before pybind11/numpy otherwise issues with ::hypot
#include <pybind11/numpy.h>

#define CM_ERROR(x)                                                                   \
    std::runtime_error((x) + std::string(__FILE__) + " " + std::to_string(__LINE__) + \
                       "\n")

typedef uint64_t u64;

namespace cliquematch
{
namespace core
{
    template <typename dtype>
    using ndarray =
        pybind11::array_t<dtype, pybind11::array::c_style | pybind11::array::forcecast>;
}  // namespace core
}  // namespace cliquematch

#endif /* CORE_H */
