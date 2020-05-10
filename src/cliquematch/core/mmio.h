#ifndef MMIO_H
#define MMIO_H

#include <vector>
#include <utility>

std::vector<std::pair<std::size_t, std::size_t>> mmio3_reader(const char* filename,
                                                              std::size_t& n_vert,
                                                              std::size_t& n_edges);

#endif /* MMIO_H */

