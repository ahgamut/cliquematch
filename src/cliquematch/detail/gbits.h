#ifndef GBITS_H
#define GBITS_H

/* gbits.h
 *
 * The graphBits class is designed to represent the sets involved in each step
 * of the clique search. It operates on an array of 32-bit values as the
 * bitset. All the single-bit/frequently-called operations are kept in the
 * header, and all the "set"-like operations are in the implementation. Think
 * of it as a slim runtime-version of std::bitset crossed with std::span from
 * C++20.
 *
 * graphBits instances DO NOT check bounds while operating on the data.
 * Wherever possible, they use their dlen attribute (i.e. ceil(number of bits
 * used/32)) when looping over the entire bitset.
 *
 * graphBits instances use a raw pointer to 32-bit data. This pointer is like
 * std::observer_ptr: can be NULL, used only to access the data, and never
 * freed.
 *
 * Earlier implementations of the class had each instance individually allocate
 * and manage memory, but that was horrible. Now all graphBits constructions DO
 * NOT perform any memory management, require the CALLER to provide correct
 * bounds, and can only be moved, not copied.
 */

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
        u32 pad_cover;          // to ensure no stray bits after the end are counted
        u32* data;              // simple pointer to external data
        std::size_t valid_len;  // number of bits used (useful for debug)
        std::size_t dlen;       // ceil(valid_len/32.0);

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
            this->data = ext_data;  // CALLER gives me the data,
            // they should have initialized it and checked bounds
            this->valid_len = n_bits;
            this->dlen = ((n_bits & 0x1fu) != 0) + (n_bits >> 5);
            this->pad_cover = ALL_ONES << (32 - (n_bits & 0x1fu));
            if (cleanout) this->clear();
        }
        void clear(std::size_t N = 0)
        {
            std::size_t i = 0;
            std::size_t clear_len = 1 + (N >> 5);
            if (N == 0 || clear_len > this->dlen) clear_len = this->dlen;
            for (i = 0; i < clear_len; i++) this->data[i] = 0;
        }

        void set(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i & 0x1fu);
            this->data[i >> 5] |= mask;
        };
        void reset(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = ~(MSB_32 >> (i & 0x1fu));
            this->data[i >> 5] &= mask;
        };
        void toggle(std::size_t i)
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i & 0x1fu);
            this->data[i >> 5] ^= mask;
        };
        bool block_empty(std::size_t i) const { return (this->data[i >> 5] == 0); };
        bool operator[](std::size_t i) const
        {
            // assert(i < this->valid_len);
            u32 mask = MSB_32 >> (i & 0x1fu);
            return (this->data[i >> 5] & mask) != 0;
        };

        // std::size_t len() const { return this->valid_len; };
        std::size_t count() const;
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
