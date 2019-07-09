#ifndef VERTEX_H
#define VERTEX_H

#include <cm_base/core/gbits.h>
#include <sstream>
#include <vector>

struct vertex {
    u32 id;    // vertex number
    u32 N;     // # neighbors + 1
    u32 spos;  // (Self-POSition) its own location in the list of neighbors
    u32 elo;   // edge list offset
    u32 ebo;   // edge list offset with bit padding

    graphBits bits;
    u32 mcs;  // *M*aximum *C*lique *S*ize

    bool operator>(const vertex& BB) const { return (this->mcs > BB.mcs); }
    bool operator<(const vertex& BB) const { return (this->mcs < BB.mcs); }

    vertex();
    vertex(u32 id, u32 N, u32 elo, u32 ebo);

    void set_spos(u32*, u32*);
    void disp(u32*);
    void disp(u32*, std::stringstream&);
    void clique_disp(u32*);
    std::vector<u32> give_clique(u32*);
};

short binary_find(u32* a, u32 N, u32 val, u32& loc);

#endif /* VERTEX_H */

