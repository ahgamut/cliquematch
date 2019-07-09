#include <vector>
#include <set>

typedef unsigned int u32;

// both of the below functions can throw a runtime error if the file does not exist
std::vector< std::set<u32> > mmio2_reader(const char* filename, u32& n_vert, u32& n_edges);
std::vector< std::set<u32> > mmio3_reader(const char* filename, u32& n_vert, u32& n_edges);
