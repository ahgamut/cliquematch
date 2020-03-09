#include <set>
#include <vector>

// both of the below functions can throw a runtime error if the file does not
// exist
std::vector<std::set<std::size_t> > mmio2_reader(const char* filename, std::size_t& n_vert,
                                         std::size_t& n_edges);
std::vector<std::set<std::size_t> > mmio3_reader(const char* filename, std::size_t& n_vert,
                                         std::size_t& n_edges);
