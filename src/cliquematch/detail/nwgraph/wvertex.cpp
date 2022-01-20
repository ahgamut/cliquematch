#include <detail/nwgraph/wvertex.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    void wvertex::disp(const u64 id, const u64* el_base) const
    {
        if (this->N <= 1 || this->mcs <= this->weight) return;
        std::cout << "Vertex " << id << " has weight = " << this->weight
                  << ", and degree " << this->degree << " from " << this->N
                  << " edges\n";
        for (u64 i = 0; i < this->N; i++) std::cerr << el_base[this->elo + i] << " ";
        std::cout << std::endl;
        std::cout << "Current Clique (weight = " << this->mcs << "):";
        this->bits.show();
        this->bits.show(&el_base[this->elo], this->N);
    }

    void wvertex::clique_disp(const u64* el_base) const
    {
        this->bits.show(&el_base[this->elo], this->N);
    }

    std::vector<u64> wvertex::give_clique(const u64* el_base) const
    {
        return this->bits.get_subset(&el_base[this->elo], this->N);
    }
}  // namespace detail
}  // namespace cliquematch
