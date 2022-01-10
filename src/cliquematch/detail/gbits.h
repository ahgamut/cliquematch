#ifndef GBITS_H
#define GBITS_H

/* gbits.h
 *
 * The graphBits class is designed to represent the sets involved in each step
 * of the clique search. It operates on an array of 64-bit values as the
 * bitset. All the single-bit/frequently-called operations are kept in the
 * header, and all the "set"-like operations are in the implementation. Think
 * of it as a slim runtime-version of std::bitset crossed with std::span from
 * C++20.
 *
 * graphBits instances DO NOT check bounds while operating on the data.
 * Wherever possible, they use their dlen attribute (i.e. ceil(number of bits
 * used/64)) when looping over the entire bitset.
 *
 * graphBits instances use a raw pointer to 64-bit data. This pointer is like
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

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanReverse64)
#endif

typedef uint64_t u64;

namespace cliquematch
{
namespace detail
{
    constexpr u64 ALL_ONES = 0xFFFFFFFFFFFFFFFFULL;
    constexpr u64 MSB_64 = 0x8000000000000000ULL;

    inline u64 clz(const u64 n)
    {
#ifdef _MSC_VER
        unsigned long index;
        _BitScanReverse64(&index, n);
        return static_cast<u64>(index);
#else
        return __builtin_clzll(n);
#endif
    }

    class graphBits
    {
       private:
        u64* data;      // simple pointer to external data
        u64 pad_cover;  // to ensure no stray bits after the end are counted
        u64 valid_len;  // number of bits used (useful for debug)

       public:
        graphBits& operator=(const graphBits&) = delete;
        graphBits() = default;
        graphBits(graphBits&& other)
        {
            this->data = other.data;
            this->valid_len = other.valid_len;
            this->pad_cover = other.pad_cover;
        }
        graphBits(u64* ext_data, const u64 n_bits, const bool cleanout = false)
        {
            this->refer_from(ext_data, n_bits, cleanout);
        }
        void copy_from(const graphBits& other, u64* data_ptr)
        {
            this->valid_len = other.valid_len;
            this->pad_cover = other.pad_cover;
            this->data = data_ptr;
            this->copy_data(other);
        }
        void copy_data(const graphBits& other)
        {
            u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
            std::copy(other.data, other.data + dlen, this->data);
        }
        void refer_from(const graphBits& other)
        {
            this->data = other.data;
            this->pad_cover = other.pad_cover;
            this->valid_len = other.valid_len;
        }
        void refer_from(u64* ext_data, const u64 n_bits, const bool cleanout = false)
        {
            this->data = ext_data;  // CALLER gives me the data,
            // they should have initialized it and checked bounds
            this->valid_len = n_bits;
            this->pad_cover = ALL_ONES << (64 - (n_bits & 0x3fu));
            if (cleanout) this->clear();
        }
        void clear(const u64 N = 0)
        {
            u64 i = 0;
            u64 clear_len = 1 + (N >> 6);
            u64 dlen = ((this->valid_len & 0x3fu) != 0) + (this->valid_len >> 6);
            if (N == 0 || clear_len > dlen) clear_len = dlen;
            for (i = 0; i < clear_len; i++) this->data[i] = 0;
        }

        void set(const u64 i)
        {
            // assert(i < this->valid_len);
            u64 mask = MSB_64 >> (i & 0x3fu);
            this->data[i >> 6] |= mask;
        };
        void reset(const u64 i)
        {
            // assert(i < this->valid_len);
            u64 mask = ~(MSB_64 >> (i & 0x3fu));
            this->data[i >> 6] &= mask;
        };
        void toggle(const u64 i)
        {
            // assert(i < this->valid_len);
            u64 mask = MSB_64 >> (i & 0x3fu);
            this->data[i >> 6] ^= mask;
        };
        bool block_empty(const u64 i) const { return (this->data[i >> 6] == 0); };
        bool operator[](const u64 i) const
        {
            // assert(i < this->valid_len);
            u64 mask = MSB_64 >> (i & 0x3fu);
            return (this->data[i >> 6] & mask) != 0;
        };
        u64 next(const u64 i)
        {
            if (i < valid_len)
            {
                const u64 base = (i >> 6);
                const u64 mask = this->data[base] & (ALL_ONES >> (i & 0x3fu));
                if (mask)
                    return (base << 6) + clz(mask);
                else
                    return next((base + 1) << 6);
            }
            else
                return this->valid_len;
        }

        // u64 len() const { return this->valid_len; };
        u64 count() const;
        graphBits& operator&=(const graphBits& other);
        graphBits& operator|=(const graphBits& other);
        graphBits& operator^=(const graphBits& other);
        graphBits& operator-=(const graphBits& other);

        void show() const;
        void show(const std::vector<u64>&) const;
        void show(const u64*, u64) const;
        std::vector<u64> get_subset(const std::vector<u64>&) const;
        std::vector<u64> get_subset(const u64*, u64) const;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* GBITS_H */
