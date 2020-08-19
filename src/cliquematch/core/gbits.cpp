#include <core/gbits.h>
#include <cassert>
#include <iostream>

namespace cliquematch
{
namespace detail
{
    inline u32 bitcount(u32 n)
    {
        n = n - ((n >> 1) & 0x55555555);
        n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
        n = (n + (n >> 4)) & 0x0F0F0F0F;
        n = n + (n >> 8);
        n = n + (n >> 16);
        return (n & 0x0000003F);
    }

    graphBits::graphBits()
    {
        this->valid_len = 0;
        this->dlen = 0;
        this->data = nullptr;
        this->ext_ptr = false;
        this->pad_cover = 0;
    }

    graphBits::graphBits(graphBits&& other)
    {
        this->data = other.data;
        this->ext_ptr = other.ext_ptr;
        this->valid_len = other.valid_len;
        this->dlen = other.dlen;
        this->pad_cover = other.pad_cover;
        // move means other will be destructed, so prevent from deleting data
        other.ext_ptr = true;
    }

    graphBits::graphBits(std::size_t n_bits)
    {
        this->valid_len = n_bits;
        this->ext_ptr = false;
        this->dlen = (n_bits % 32 != 0) + n_bits / 32;
        this->pad_cover = n_bits % 32 == 0 ? ALL_ONES : ALL_ONES << (32 - n_bits % 32);
        this->data = new u32[this->dlen];
        for (std::size_t i = 0; i < this->dlen; ++i) this->data[i] = 0;
    }

    void graphBits::refer_from(u32* ext_data, std::size_t n_bits, bool cleanout)
    {
        this->data = ext_data;  // since someone else gives me the data, they should
                                // have inited
        this->ext_ptr = true;
        this->valid_len = n_bits;
        this->dlen = (n_bits % 32 != 0) + n_bits / 32;
        this->pad_cover = n_bits % 32 == 0 ? ALL_ONES : ALL_ONES << (32 - n_bits % 32);
        if (cleanout) this->clear();
    }

    void graphBits::copy_from(const graphBits& other)
    {
        if (!this->ext_ptr && this->data != nullptr) delete[] this->data;
        this->dlen = other.dlen;
        this->valid_len = other.valid_len;
        this->pad_cover = other.pad_cover;
        this->ext_ptr = false;
        this->data = new u32[other.dlen];
        this->copy_data(other);
    }

    void graphBits::copy_data(const graphBits& other)
    {
        std::copy(other.data, other.data + this->dlen, this->data);
    }

    void graphBits::clear(std::size_t N)
    {
        std::size_t i = 0;
        std::size_t clear_len;
        if (N == 0 || N >= this->valid_len || 1 + (N / 32) >= this->dlen)
            clear_len = this->dlen;
        else
            clear_len = 1 + N / 32;
        for (i = 0; i < clear_len; i++) this->data[i] = 0;
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
        { this->data[i] &= ~(other.data[i]); }
        this->data[this->dlen - 1] &= this->pad_cover;
        return *this;
    }

    graphBits graphBits::operator~() const
    {
        graphBits temp(this->valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
        { temp.data[i] = ~(this->data[i]); }
        // do I need to worry about what the padding bits are?
        // YES, for the popcnt stuff
        temp.data[this->dlen - 1] &= this->pad_cover;
        return temp;
    }

    graphBits graphBits::operator&(const graphBits& other) const
    {
        // assert(this->valid_len == other.valid_len);
        graphBits temp(this->valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
            temp.data[i] = this->data[i] & other.data[i];
        temp.data[temp.dlen - 1] &= temp.pad_cover;
        return temp;
    }

    graphBits graphBits::operator|(const graphBits& other) const
    {
        // assert(this->valid_len == other.valid_len);
        graphBits temp(this->valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
            temp.data[i] = this->data[i] | other.data[i];
        temp.data[temp.dlen - 1] &= temp.pad_cover;
        return temp;
    }

    graphBits graphBits::operator^(const graphBits& other) const
    {
        // assert(this->valid_len == other.valid_len);
        graphBits temp(this->valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
            temp.data[i] = this->data[i] ^ other.data[i];
        temp.data[temp.dlen - 1] &= temp.pad_cover;
        return temp;
    }

    graphBits graphBits::operator-(const graphBits& other) const
    {
        // assert(this->valid_len == other.valid_len);
        graphBits temp(this->valid_len);
        for (std::size_t i = 0; i < this->dlen; i++)
            temp.data[i] = this->data[i] & ~(other.data[i]);
        temp.data[temp.dlen - 1] &= temp.pad_cover;
        return temp;
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
