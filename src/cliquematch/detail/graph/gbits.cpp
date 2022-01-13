/* gbits.cpp
 *
 * Implementation of all the "set"-like operations in the graphBits class,
 * along with functions to display/return the set being represented.
 */

#include <detail/graph/gbits.h>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    // returns the number of bits on in n
    static inline u64 bitcount(u64 n)
    {
        n = n - ((n >> 1) & 0x5555555555555555);
        n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333);
        n = (n + (n >> 4)) & 0x0f0f0f0f0f0f0f0f;
        n = n + (n >> 8);
        n = n + (n >> 16);
        n = n + (n >> 32);
        return (n & 0x7f);
    }

    u64 graphBits::count() const
    {
        u64 sum = 0;
        u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
        this->data[dlen - 1] &= this->pad_cover;
        for (u64 i = 0; i < dlen; i++) sum += bitcount(this->data[i]);
        return sum;
    }

    graphBits& graphBits::operator&=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
        for (u64 i = 0; i < dlen; i++) { this->data[i] &= other.data[i]; }
        this->data[dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator|=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
        for (u64 i = 0; i < dlen; i++) { this->data[i] |= other.data[i]; }
        this->data[dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator^=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
        for (u64 i = 0; i < dlen; i++) { this->data[i] ^= other.data[i]; }
        this->data[dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits& graphBits::operator-=(const graphBits& other)
    {
        // assert(this->valid_len == other.valid_len);
        u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
        for (u64 i = 0; i < dlen; i++) { this->data[i] &= ~(other.data[i]); }
        this->data[dlen - 1] &= this->pad_cover;
        return *this;
    }

    void graphBits::show() const
    {
        for (u64 i = 0; i < this->valid_len; i++) std::cout << (*this)[i];
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    void graphBits::show(const std::vector<u64>& elements) const
    {
        // assert(this->valid_len <= elements.size());
        for (u64 i = 0; i < this->valid_len; i++)
        {
            if ((*this)[i]) std::cout << elements[i] << " ";
        }
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    void graphBits::show(const u64* elements, u64 len) const
    {
        // assert(this->valid_len <= len);
        for (u64 i = 0; i < this->valid_len; i++)
        {
            if ((*this)[i]) std::cout << elements[i] << " ";
        }
        std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
    }

    std::vector<u64> graphBits::get_subset(const std::vector<u64>& elements) const
    {
        // assert(this->valid_len <= elements.size());
        std::vector<u64> ans(this->count());
        for (u64 i = 0, ct = 0; i < this->valid_len; i++)
        {
            if ((*this)[i])
            {
                ans[ct] = elements[i];
                ct++;
            }
        }
        return ans;
    }

    std::vector<u64> graphBits::get_subset(const u64* elements, u64 len) const
    {
        // assert(this->valid_len <= len);
        std::vector<u64> ans(this->count());
        for (u64 i = 0, ct = 0; i < this->valid_len; i++)
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
