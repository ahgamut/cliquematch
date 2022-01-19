#ifndef MMIO_H
#define MMIO_H

#include <vector>
#include <utility>
#include <cstdint>

namespace cliquematch
{
namespace detail
{
    typedef uint64_t u64;
    // reads the given file as text following the Matrix Market format
    //
    // https://math.nist.gov/MatrixMarket/formats.html.
    //
    // returns loaded edges
    // number of vertices/edges are also read from file, returned by reference
    std::pair<std::vector<u64>, std::vector<u64>> mmio4_reader(const char* filename,
                                                               u64& n_vert,
                                                               u64& n_edges);
    // throws error if unable to read the file,
    // prints to stderr if invalid edges are found (ignored in graph)
}  // namespace detail
}  // namespace cliquematch
#endif /* MMIO_H */
