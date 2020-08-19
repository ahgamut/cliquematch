#include <core/vertex.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    vertex::vertex()
    {
        this->id = 0;
        this->N = 0;
        this->spos = 0;
        this->elo = 0;
        this->ebo = 0;
        this->mcs = 0;
    }

    void vertex::refer_from(std::size_t id, std::size_t N, std::size_t elo,
                            std::size_t ebo)
    {
        this->id = id;
        this->N = N;
        this->elo = elo;
        this->ebo = ebo;
        this->mcs = 1;
        this->spos = 0;
    }

    void vertex::set_spos(std::size_t* el_base, u32* eb_base)
    {
        // no need to check returned value because it is always 1
        binary_find(&el_base[this->elo], this->N, this->id, this->spos);
        this->bits.refer_from(&eb_base[this->ebo], this->N);
        this->bits.set(this->spos);
    }

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
