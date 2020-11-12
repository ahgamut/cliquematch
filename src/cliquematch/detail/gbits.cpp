/* gbits.cpp
 *
 * Implementation of all the "set"-like operations in the graphBits class,
 * along with functions to display/return the set being represented.
 */

#include <detail/gbits.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    // returns the number of bits on in n
    inline u32 bitcount(u32 n)
    {
        n = n - ((n >> 1) & 0x55555555);
        n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
        n = (n + (n >> 4)) & 0x0F0F0F0F;
        n = n + (n >> 8);
        n = n + (n >> 16);
        return (n & 0x0000003F);
    }

    std::size_t graphBits::count() const
    {
        std::size_t sum = 0;
        this->data[this->dlen - 1] &= this->pad_cover;
        for (std::size_t i = 0; i < this->dlen; i++) sum += bitcount(this->data[i]);
        return sum;
    }

    graphBits& graphBits::operator&=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        for (std::size_t i = 0; i < this->dlen; i++) { this->data[i] &= other.data[i]; }
        this->data[this->dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator|=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        for (std::size_t i = 0; i < this->dlen; i++) { this->data[i] |= other.data[i]; }
        this->data[this->dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator^=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        for (std::size_t i = 0; i < this->dlen; i++) { this->data[i] ^= other.data[i]; }
        this->data[this->dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator-=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
        {
            this->data[i] &= ~(other.data[i]);
        }
        this->data[this->dlen - 1] &= this->pad_cover;
        return *this;
    }

    void graphBits::show() const
    {
        for (std::size_t i = 0; i < this->valid_len; i++) std::cout << (*this)[i];
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    void graphBits::show(const std::vector<std::size_t>& elements) const
    {
        // assert(this->valid_len <= elements.size());
        for (std::size_t i = 0; i < this->valid_len; i++)
        {
            if ((*this)[i]) std::cout << elements[i] << " ";
        }
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    void graphBits::show(const std::size_t* elements, std::size_t len) const
    {
        // assert(this->valid_len <= len);
        for (std::size_t i = 0; i < this->valid_len; i++)
        {
            if ((*this)[i]) std::cout << elements[i] << " ";
        }
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    std::vector<std::size_t> graphBits::get_subset(
        const std::vector<std::size_t>& elements) const
    {
        // assert(this->valid_len <= elements.size());
        std::vector<std::size_t> ans(this->count());
        for (std::size_t i = 0, ct = 0; i < this->valid_len; i++)
        {
            if ((*this)[i])
            {
                ans[ct] = elements[i];
                ct++;
            }
        }
        return ans;
    }

    std::vector<std::size_t> graphBits::get_subset(const std::size_t* elements,
                                                   std::size_t len) const
    {
        // assert(this->valid_len <= len);
        std::vector<std::size_t> ans(this->count());
        for (std::size_t i = 0, ct = 0; i < this->valid_len; i++)
        {
            if ((*this)[i])
            {
                ans[ct] = elements[i];
                ct++;
            }
        }
        return ans;
    }

}  // namespace detail
}  // namespace cliquematch
