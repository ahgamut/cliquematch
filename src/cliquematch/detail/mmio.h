#ifndef MMIO_H
#define MMIO_H

#include <vector>
#include <utility>

namespace cliquematch
{
namespace detail
{
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> mmio4_reader(
        const char* filename, std::size_t& n_vert, std::size_t& n_edges);
}  // namespace detail
}  // namespace cliquematch
#endif /* MMIO_H */

