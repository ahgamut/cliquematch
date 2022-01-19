#ifndef VERTEX_H
#define VERTEX_H

/* vertex.h
 *
 * the vertex class contains all the contextual information required to
 * represent data related to one vertex in the graph. the raw data is stored in
 * the edge_list and edge_bits attributes of the graph instance, so each vertex
 * instance v_i stores the offsets, vertex ID, degree, and a clique search bound.
 *
 * To easily represent the clique, each vertex is considered its own neighbor.
 * This adds some (unnecessary) space, but speeds up the clique search.
 * spos is the position of a vertex in its list of neighbors.
 *
 * The data members are used mainly to easily access/manipulate the raw data
 * given by the graph class (search for valid neighbors and update clique
 * data).
 *
 * vertex instances do not manage any memory, can be default constructed, and
 * require the CALLER to ensure the data they refer to are valid.
 */

#include <detail/gbits.h>

namespace cliquematch
{
namespace detail
{
    struct vertex
    {
        u64 N;     // # neighbors + 1 (the vertex itself)
        u64 spos;  // (Self-POSition) its own location in the list of neighbors
        u64 elo;   // edge_list offset
        u64 ebo;   // edge_bits offset (with correct padding)
        u64 mcs;   // *M*aximum *C*lique *S*ize for a search at this vertex

        graphBits bits;

        vertex()
        {
            this->N = 0;
            this->spos = 0;
            this->elo = 0;
            this->ebo = 0;
            this->mcs = 0;
        }
        // store degree and offsets
        void refer_from(u64 N, u64 elo, u64 ebo)
        {
            this->N = N;
            this->elo = elo;
            this->ebo = ebo;
            this->mcs = 1;
            this->spos = 0;
        }
        // compute spos and load bitset data
        void set_spos(u64* eb_base, u64 spos)
        {
            // assert(el_base[this->elo + spos] == this->id);
            this->spos = spos;
            this->bits.refer_from(&eb_base[this->ebo], this->N);
            this->bits.set(this->spos);
        }

        // display the neighbors given the raw data
        void disp(const u64, const u64*) const;
        void clique_disp(const u64*) const;

        // return a clique computed starting at this vertex
        std::vector<u64> give_clique(const u64*) const;
    };

}  // namespace detail
}  // namespace cliquematch
#endif /* VERTEX_H */
