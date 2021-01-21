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
    /* this function is called to check if two vertices are neighbors
     * the parameters are usually the list of neighbors of a vertex,
     * the vertex degree, a potential neighbor, and the location of the neighbor
     */
    inline short binary_find(const std::size_t* const a, const std::size_t N,
                             const std::size_t val, std::size_t& loc)
    {
        /* modified binary search, returns location by reference
         * return 1 if found, 0 if not found
         * returns -1 only if value is greater than the array
         */
        std::size_t beg = 0, end = N - 1, mid = beg + ((end - beg) >> 1);
        if (a[end] < val)
        {
            loc = end;
            return -1;
        }
        else if (a[beg] > val)
        {
            loc = beg;
            return 0;
        }
        while (beg <= end)
        {
            if (a[mid] == val)
            {
                loc = mid;
                return 1;
            }
            else if (a[mid] < val)
                beg = mid + 1;
            else
                end = mid - 1;
            mid = beg + ((end - beg) >> 1);
        }
        return 0;
    }

    struct vertex
    {
        std::size_t id;    // vertex number
        std::size_t N;     // # neighbors + 1 (the vertex itself)
        std::size_t spos;  // (Self-POSition) its own location in the list of neighbors
        std::size_t elo;   // edge_list offset
        std::size_t ebo;   // edge_bits offset (with correct padding)
        std::size_t mcs;   // *M*aximum *C*lique *S*ize for a search at this vertex

        graphBits bits;

        // operators provided in case sorting is used to speed up the search
        bool operator>(const vertex& BB) const { return (this->mcs > BB.mcs); }
        bool operator<(const vertex& BB) const { return (this->mcs < BB.mcs); }

        vertex()
        {
            this->id = 0;
            this->N = 0;
            this->spos = 0;
            this->elo = 0;
            this->ebo = 0;
            this->mcs = 0;
        }
        // store degree and offsets
        void refer_from(std::size_t id, std::size_t N, std::size_t elo, std::size_t ebo)
        {
            this->id = id;
            this->N = N;
            this->elo = elo;
            this->ebo = ebo;
            this->mcs = 1;
            this->spos = 0;
        }
        // compute spos and load bitset data
        void set_spos(std::size_t* el_base, u32* eb_base)
        {
            // no need to check returned value because it should always be 1
            binary_find(&el_base[this->elo], this->N, this->id, this->spos);
            this->bits.refer_from(&eb_base[this->ebo], this->N);
            this->bits.set(this->spos);
        }
        // display the neighbors given the raw data
        void disp(const std::size_t*) const;
        void clique_disp(const std::size_t*) const;
        // return a clique computed starting at this vertex
        std::vector<std::size_t> give_clique(const std::size_t*) const;
    };

}  // namespace detail
}  // namespace cliquematch
#endif /* VERTEX_H */

