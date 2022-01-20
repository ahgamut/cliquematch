#ifndef WVERTEX_H
#define WVERTEX_H

#include <detail/gbits.h>

namespace cliquematch
{
namespace detail
{
    struct wvertex  // weighted vertex
    {
        u64 N;     // # neighbors + 1 (the vertex itself)
        u64 spos;  // (Self-POSition) its own location in the list of neighbors
        u64 elo;   // edge_list offset
        u64 ebo;   // edge_bits offset (with correct padding)
        double weight;
        double degree;
        double mcs;  // *M*aximum *C*lique *S*ize for a search at this vertex
        graphBits bits;

        void load(u64 N, u64 elo, u64 ebo, u64* eb_base, u64 spos, double wt,
                  double deg)
        {
            this->N = N;
            this->elo = elo;
            this->ebo = ebo;
            this->spos = spos;
            this->weight = wt;
            this->degree = deg;
            this->mcs = wt;
            this->bits.refer_from(&(eb_base[this->ebo]), this->N);
            this->bits.set(this->spos);
        }

        void disp(const u64, const u64*) const;
        void clique_disp(const u64*) const;
        std::vector<u64> give_clique(const u64*) const;
    };
}  // namespace detail
}  // namespace cliquematch

#endif /* WVERTEX_H */
