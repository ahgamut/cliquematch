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
        bool ext_ptr;  // true if someone else gave the data pointer
        u32 pad_cover;
        u32* data;
        std::size_t valid_len, dlen;

       public:
        graphBits();
        graphBits(std::size_t n_bits);
        graphBits(graphBits&& other);
        graphBits(const graphBits&) = delete;
        graphBits& operator=(const graphBits&) = delete;
        ~graphBits()
        {
            if (!this->ext_ptr && this->data != nullptr)
            {
                delete[] this->data;
                this->data = nullptr;
            }
        };

        void copy_from(const graphBits& other);
        void copy_data(const graphBits& other);
        void refer_from(u32* ext_data, std::size_t n_bits, bool cleanout = true);
        void refer_from(const graphBits& other)
        {
            refer_from(other.data, other.valid_len, false);
        };

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

        void clear(std::size_t N = 0);
        std::size_t count() const;
        std::size_t len() const { return this->valid_len; };
        graphBits& operator&=(const graphBits& other);
        graphBits& operator|=(const graphBits& other);
        graphBits& operator^=(const graphBits& other);
        graphBits& operator-=(const graphBits& other);

        graphBits operator~() const;
        graphBits operator&(const graphBits& other) const;
        graphBits operator|(const graphBits& other) const;
        graphBits operator^(const graphBits& other) const;
        graphBits operator-(const graphBits& other) const;

        void show() const;
        void show(const std::vector<std::size_t>&) const;
        void show(const std::size_t*, std::size_t) const;
        std::vector<std::size_t> get_subset(const std::vector<std::size_t>&) const;
        std::vector<std::size_t> get_subset(const std::size_t*, std::size_t) const;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* GBITS_H */
