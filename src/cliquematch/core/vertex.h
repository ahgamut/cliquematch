#ifndef VERTEX_H
#define VERTEX_H

#include <core/gbits.h>
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
    void load_external(u32 id, u32 N, u32 elo, u32 ebo);
    void set_spos(u32*, u32*);

    void disp(const u32*) const;
    void clique_disp(const u32*) const;
    std::vector<u32> give_clique(const u32*) const;
};

short binary_find(const u32* a, u32 N, u32 val, u32& loc);

#endif /* VERTEX_H */

