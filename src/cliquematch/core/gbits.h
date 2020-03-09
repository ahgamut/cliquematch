#ifndef GBITS_H
#define GBITS_H

#include <vector>
#include <cstdint>

typedef uint32_t u32;
constexpr u32 ALL_ONES = 0xFFFFFFFF;
constexpr u32 MSB_32 = 0x80000000;

class graphBits {
   private:
    u32* data;
    bool ext_ptr;  // true if someone else gave the data pointer
	std::size_t valid_len, dlen; 
	u32 pad_cover;

   public:
    graphBits();

    graphBits(std::size_t n_bits);
    void load_external(u32* ext_data, std::size_t n_bits, bool cleanout = true);
    virtual ~graphBits() {
	if (!this->ext_ptr && this->data != nullptr) {
	    delete[] this->data;
	    this->data = nullptr;
	}
    };
    void set(std::size_t i);
    void reset(std::size_t i);
    void toggle(std::size_t i);
    void clear(std::size_t N = 0);
    std::size_t count() const;

    bool block_empty(std::size_t i) const;
    bool operator[](std::size_t i) const;

    graphBits(const graphBits& other);
    graphBits(graphBits&& other);
    friend void swap(graphBits& me, graphBits& other);
    graphBits& operator=(graphBits other);

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

#endif /* GBITS_H */
