#include <core/vertex.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    short binary_find(const std::size_t* const a, const std::size_t N,
                      const std::size_t val, std::size_t& loc)
    {
        // modified binary search, returns location by reference
        // return 1 if found, 0 if not found
        // returns -1 only if value is greater than the array
        std::size_t beg = 0, end = N - 1, mid = (beg + end / 2);
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
            mid = (beg + end) / 2;
        }
        return 0;
    }

    vertex::vertex()
    {
        this->id = 0;
        this->N = 0;
        this->spos = 0;
        this->elo = 0;
        this->ebo = 0;
        this->mcs = 0;
    }

    void vertex::load_external(std::size_t id, std::size_t N, std::size_t elo,
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
        this->bits.load_external(&eb_base[this->ebo], this->N);
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
