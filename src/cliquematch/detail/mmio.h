#ifndef MMIO_H
#define MMIO_H

#include <vector>
#include <utility>

namespace cliquematch
{
namespace detail
{
    // reads the given file as text following the Matrix Market format
    //
    // https://math.nist.gov/MatrixMarket/formats.html.
    //
    // returns loaded edges
    // number of vertices/edges are also read from file, returned by reference
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> mmio4_reader(
        const char* filename, std::size_t& n_vert, std::size_t& n_edges);
    // throws error if unable to read the file,
    // prints to stderr if invalid edges are found (ignored in graph)
}  // namespace detail
}  // namespace cliquematch
#endif /* MMIO_H */

