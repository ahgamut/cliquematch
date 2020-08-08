#ifndef VERTEX_H
#define VERTEX_H

#include <core/gbits.h>

namespace cliquematch
{
namespace detail
{
    struct vertex
    {
        std::size_t id;    // vertex number
        std::size_t N;     // # neighbors + 1
        std::size_t spos;  // (Self-POSition) its own location in the list of neighbors
        std::size_t elo;   // edge list offset
        std::size_t ebo;   // edge list offset with bit padding
        std::size_t mcs;   // *M*aximum *C*lique *S*ize

        graphBits bits;

        bool operator>(const vertex& BB) const { return (this->mcs > BB.mcs); }
        bool operator<(const vertex& BB) const { return (this->mcs < BB.mcs); }

        vertex();
        void load_external(std::size_t id, std::size_t N, std::size_t elo,
                           std::size_t ebo);
        void set_spos(std::size_t*, u32*);

        void disp(const std::size_t*) const;
        void clique_disp(const std::size_t*) const;
        std::vector<std::size_t> give_clique(const std::size_t*) const;
    };

    short binary_find(const std::size_t* const a, const std::size_t N,
                      const std::size_t val, std::size_t& loc);
}  // namespace detail
}  // namespace cliquematch
#endif /* VERTEX_H */

