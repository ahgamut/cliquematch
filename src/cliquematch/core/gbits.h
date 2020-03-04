#ifndef GBITS_H
#define GBITS_H

#include <vector>
typedef unsigned int u32;
constexpr unsigned int ALL_ONES = 0xFFFFFFFF;
constexpr unsigned int MSB_32 = 0x80000000;

class graphBits {
   private:
    u32* data;
    u32 valid_len, dlen, pad_cover;
    bool ext_ptr;  // true if someone else gave the data pointer

   public:
    graphBits();

    graphBits(u32 n_bits);
    void load_external(u32* ext_data, u32 n_bits, bool cleanout = true);
    virtual ~graphBits() {
	if (!this->ext_ptr && this->data != nullptr) {
	    delete[] this->data;
	    this->data = nullptr;
	}
    };
    void set(u32 i);
    void reset(u32 i);
    void toggle(u32 i);
    void clear(u32 N = 0);
    u32 count() const;

    bool block_empty(u32 i) const;
    bool operator[](u32 i) const;

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
    void show(const std::vector<u32>&) const;
    void show(const u32*, u32) const;

    std::vector<u32> get_subset(const std::vector<u32>&) const;
    std::vector<u32> get_subset(const u32*, u32) const;
};

#endif /* GBITS_H */
