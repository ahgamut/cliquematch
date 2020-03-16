#include <core/gbits.h>
#include <cassert>
#include <iostream>

/* Using Intrinsics:
 *
 * To count the number of set bits in an unsigned int, gcc and msvc offer a
 * compiler intrinsic (__builtin_popcount and __popcnt respectively). The
 * intrinsics are slightly faster, but may cause compatibility issues, so there
 * is a C version also provided, to count the number of set bits.
 * */
#ifdef INTRINSIC_BITCOUNT

#ifdef _MSC_VER
#include <intrin.h>
#define bitcount __popcnt
#else
#define bitcount __builtin_popcount
#endif

#else
inline u32 arith_bcount(u32 n) {
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0F0F0F0F;
    n = n + (n >> 8);
    n = n + (n >> 16);
    return (n & 0x0000003F);
}

#define bitcount arith_bcount
#endif

graphBits::graphBits() {
    this->valid_len = 0;
    this->dlen = 0;
    this->data = nullptr;
    this->ext_ptr = false;
    this->pad_cover = 0;
}

graphBits::graphBits(std::size_t n_bits) {
    this->valid_len = n_bits;
    this->dlen = 1 + n_bits / 32;
    this->data = new u32[this->dlen];
    for (unsigned int i = 0; i < this->dlen; ++i) this->data[i] = 0;
    this->ext_ptr = false;
    this->pad_cover = n_bits % 32 == 0 ? 0 : ALL_ONES << (32 - n_bits % 32);
}

void graphBits::load_external(u32* ext_data, std::size_t n_bits, bool cleanout) {
    this->data = ext_data;  // since someone else gives me the data, they should
			    // have inited
    this->valid_len = n_bits;
    this->dlen = 1 + n_bits / 32;
    this->ext_ptr = true;
    if (cleanout) this->clear();
    this->pad_cover = n_bits % 32 == 0 ? 0 : ALL_ONES << (32 - n_bits % 32);
}

graphBits::graphBits(const graphBits& other)
    : valid_len(other.valid_len), dlen(other.dlen), pad_cover(other.pad_cover) {
    this->data = new u32[other.dlen];
    std::copy(other.data, other.data + other.dlen, this->data);
    this->ext_ptr = false;
}

graphBits& graphBits::operator=(graphBits other) {
    swap(*this, other);
    return *this;
}

graphBits::graphBits(graphBits&& other) : graphBits() { swap(*this, other); }

void swap(graphBits& me, graphBits& other) {
    me.valid_len = other.valid_len;
    me.dlen = other.dlen;
    me.pad_cover = other.pad_cover;

    if (me.ext_ptr) {
	std::copy(other.data, other.data + other.dlen, me.data);
    } else {
	if (me.data != nullptr) delete[] me.data;
	me.data = other.data;
	if (!other.ext_ptr) other.ext_ptr = true;
	// I expect the other to die before me
	else  // I shouldn't ruin others' data
	    me.ext_ptr = true;
    }
}

void graphBits::set(std::size_t i) {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    this->data[i / 32] |= mask;
}

void graphBits::reset(std::size_t i) {
    // assert(i < this->valid_len);
    u32 mask = ~(MSB_32 >> (i % 32));
    this->data[i / 32] &= mask;
}

void graphBits::toggle(std::size_t i) {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    this->data[i / 32] ^= mask;
}

void graphBits::clear(std::size_t N) {
    std::size_t i = 0;
    if (N == 0 || N == this->valid_len || N / 32 == this->dlen) {
	for (i = 0; i < this->dlen; i++) this->data[i] = 0;
    } else {
	this->data[i] &= (ALL_ONES >> N % 32);
    }
}

std::size_t graphBits::count() const {
	std::size_t sum = 0;
    this->data[this->dlen - 1] &= this->pad_cover;
    for (std::size_t i = 0; i < this->dlen; i++) sum += bitcount(this->data[i]);
    return sum;
}

bool graphBits::block_empty(std::size_t i) const { return (this->data[i / 32] == 0); }

bool graphBits::operator[](std::size_t i) const {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    return (this->data[i / 32] & mask) != 0;
}

graphBits& graphBits::operator&=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (std::size_t i = 0; i < this->dlen; i++) {
	this->data[i] &= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;
    return *this;
}

graphBits& graphBits::operator|=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (std::size_t i = 0; i < this->dlen; i++) {
	this->data[i] |= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;
    return *this;
}

graphBits& graphBits::operator^=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (std::size_t i = 0; i < this->dlen; i++) {
	this->data[i] ^= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;
    return *this;
}

graphBits& graphBits::operator-=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (std::size_t i = 0; i < this->dlen; i++) {
	this->data[i] &= ~(other.data[i]);
    }
    this->data[this->dlen - 1] &= this->pad_cover;
    return *this;
}

graphBits graphBits::operator~() const {
    graphBits temp(this->valid_len);
    for (std::size_t i = 0; i < this->dlen; i++) {
	temp.data[i] = ~(this->data[i]);
    }
    // do I need to worry about what the padding bits are?
    // YES, for the popcnt stuff
    temp.data[this->dlen - 1] &= this->pad_cover;
    return temp;
}

graphBits graphBits::operator&(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    graphBits temp(this->valid_len);
    for (std::size_t i = 0; i < this->dlen; i++)
	temp.data[i] = this->data[i] & other.data[i];
    temp.data[temp.dlen - 1] &= temp.pad_cover;
    return temp;
}

graphBits graphBits::operator|(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    graphBits temp(this->valid_len);
    for (std::size_t i = 0; i < this->dlen; i++)
	temp.data[i] = this->data[i] | other.data[i];
    temp.data[temp.dlen - 1] &= temp.pad_cover;
    return temp;
}

graphBits graphBits::operator^(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    graphBits temp(this->valid_len);
    for (std::size_t i = 0; i < this->dlen; i++)
	temp.data[i] = this->data[i] ^ other.data[i];
    temp.data[temp.dlen - 1] &= temp.pad_cover;
    return temp;
}

graphBits graphBits::operator-(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    graphBits temp(this->valid_len);
    for (std::size_t i = 0; i < this->dlen; i++)
	temp.data[i] = this->data[i] & ~(other.data[i]);
    temp.data[temp.dlen - 1] &= temp.pad_cover;
    return temp;
}

void graphBits::show() const {
    for (std::size_t i = 0; i < this->valid_len; i++) std::cout << (*this)[i];
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

void graphBits::show(const std::vector<std::size_t>& elements) const {
    // assert(this->valid_len <= elements.size());
    for (std::size_t i = 0; i < this->valid_len; i++) {
	if ((*this)[i]) std::cout << elements[i] << " ";
    }
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

void graphBits::show(const std::size_t* elements, std::size_t len) const {
    // assert(this->valid_len <= len);
    for (std::size_t i = 0; i < this->valid_len; i++) {
	if ((*this)[i]) std::cout << elements[i] << " ";
    }
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

std::vector<std::size_t> graphBits::get_subset(const std::vector<std::size_t>& elements) const {
    // assert(this->valid_len <= elements.size());
    std::vector<std::size_t> ans(this->count());
    for (std::size_t i = 0, ct = 0; i < this->valid_len; i++) {
	if ((*this)[i]) {
	    ans[ct] = elements[i];
	    ct++;
	}
    }
    return ans;
}

std::vector<std::size_t> graphBits::get_subset(const std::size_t* elements, std::size_t len) const {
    // assert(this->valid_len <= len);
    std::vector<std::size_t> ans(this->count());
    for (std::size_t i = 0, ct = 0; i < this->valid_len; i++) {
	if ((*this)[i]) {
	    ans[ct] = elements[i];
	    ct++;
	}
    }
    return ans;
}
