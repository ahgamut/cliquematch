/* vertex.cpp
 *
 * Contains functions to display/return cliques corresponding to the given vertex
 * instance.
 */
#include <detail/graph/vertex.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    void vertex::disp(const u64 id, const u64* el_base) const
    {
        if (this->N <= 1 || this->mcs <= 1) return;
        std::cout << "Vertex " << id << " has " << this->N << " edges\n";
        for (u64 i = 0; i < this->N; i++) std::cerr << el_base[this->elo + i] << " ";
        std::cout << "Current Clique: ";
        this->bits.show();
        this->bits.show(&el_base[this->elo], this->N);
    }

    void vertex::clique_disp(const u64* el_base) const
    {
        this->bits.show(&el_base[this->elo], this->N);
    }

    std::vector<u64> vertex::give_clique(const u64* el_base) const
    {
        return this->bits.get_subset(&el_base[this->elo], this->N);
    }
}  // namespace detail
}  // namespace cliquematch
