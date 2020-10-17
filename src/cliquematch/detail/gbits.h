#ifndef GBITS_H
#define GBITS_H

#include <vector>
#include <cstdint>

namespace cliquematch
{
namespace detail
{
    typedef uint32_t u32;
    constexpr u32 ALL_ONES = 0xFFFFFFFF;
    constexpr u32 MSB_32 = 0x80000000;

    class graphBits
    {
       private:
        u32 pad_cover;
        u32* data;
        std::size_t valid_len, dlen;

       public:
        graphBits& operator=(const graphBits&) = delete;
        graphBits() = default;
        graphBits(graphBits&& other)
        {
            this->data = other.data;
            this->valid_len = other.valid_len;
            this->dlen = other.dlen;
            this->pad_cover = other.pad_cover;
        }
        graphBits(u32* ext_data, std::size_t n_bits, bool cleanout = false)
        {
            this->refer_from(ext_data, n_bits, cleanout);
        }
        void copy_from(const graphBits& other, u32* data_ptr)
        {
            this->dlen = other.dlen;
            this->valid_len = other.valid_len;
            this->pad_cover = other.pad_cover;
            this->data = data_ptr;
            this->copy_data(other);
        }
        void copy_data(const graphBits& other)
        {
            std::copy(other.data, other.data + this->dlen, this->data);
        }
        void refer_from(const graphBits& other)
        {
            this->data = other.data;
            this->pad_cover = other.pad_cover;
            this->valid_len = other.valid_len;
            this->dlen = other.dlen;
        }
        void refer_from(u32* ext_data, std::size_t n_bits, bool cleanout = false)
        {
            this->data = ext_data;  // since someone else gives me the data, they should
                                    // have inited
            this->valid_len = n_bits;
            this->dlen = (n_bits % 32 != 0) + n_bits / 32;
            this->pad_cover =
                n_bits % 32 == 0 ? ALL_ONES : ALL_ONES << (32 - n_bits % 32);
            if (cleanout) this->clear();
        }
        void clear(std::size_t N = 0)
        {
            std::size_t i = 0;
            std::size_t clear_len;
            if (N == 0 || N >= this->valid_len || 1 + (N / 32) >= this->dlen)
                clear_len = this->dlen;
            else
                clear_len = 1 + N / 32;
            for (i = 0; i < clear_len; i++) this->data[i] = 0;
        }

        void set(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i % 32);
            this->data[i / 32] |= mask;
        };
        void reset(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = ~(MSB_32 >> (i % 32));
            this->data[i / 32] &= mask;
        };
        void toggle(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i % 32);
            this->data[i / 32] ^= mask;
        };
        bool block_empty(std::size_t i) const { return (this->data[i / 32] == 0); };
        bool operator[](std::size_t i) const
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i % 32);
            return (this->data[i / 32] & mask) != 0;
        };

        std::size_t count() const;
        std::size_t len() const { return this->valid_len; };
        graphBits& operator&=(const graphBits& other);
        graphBits& operator|=(const graphBits& other);
        graphBits& operator^=(const graphBits& other);
        graphBits& operator-=(const graphBits& other);

        void show() const;
        void show(const std::vector<std::size_t>&) const;
        void show(const std::size_t*, std::size_t) const;
        std::vector<std::size_t> get_subset(const std::vector<std::size_t>&) const;
        std::vector<std::size_t> get_subset(const std::size_t*, std::size_t) const;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* GBITS_H */
