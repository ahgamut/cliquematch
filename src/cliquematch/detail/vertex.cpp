/* vertex.cpp
 *
 * Contains functions to display/return cliques corresponding to the given vertex
 * instance.
 */
#include <detail/vertex.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    void vertex::disp(const std::size_t* el_base) const
    {
        if (this->N <= 1 || this->mcs <= 1) return;
        std::cout << "Vertex " << this->id << " has " << this->N << " edges\n";
        for (std::size_t i = 0; i < this->N; i++)
            std::cerr << el_base[this->elo + i] << " ";
        std::cout << "Current Clique: ";
        this->bits.show();
        this->bits.show(&el_base[this->elo], this->N);
    }

    void vertex::clique_disp(const std::size_t* el_base) const
    {
        this->bits.show(&el_base[this->elo], this->N);
    }

    std::vector<std::size_t> vertex::give_clique(const std::size_t* el_base) const
    {
        return this->bits.get_subset(&el_base[this->elo], this->N);
    }
}  // namespace detail
}  // namespace cliquematch
