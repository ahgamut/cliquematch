#include <cm_base/include/core/gbits.h>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>

#ifdef _MSC_VER
#include <intrin.h>
#define __builtin_popcount __popcnt
#endif

/* Can I have a version that doesn't use __popcnt:  <05-06-19, gautham> */

graphBits::graphBits() {
    this->valid_len = 0;
    this->dlen = 0;
    this->data = nullptr;
    this->ext_ptr = true;
    this->pad_cover = 0;
}

graphBits::graphBits(u32 n_bits) {
    this->valid_len = n_bits;
    this->dlen = 1 + n_bits / 32;
    this->data = new u32[this->dlen];
    for (u32 i = 0; i < this->dlen; i++) this->data[i] = 0;
    this->ext_ptr = false;
    this->pad_cover = n_bits % 32 == 0 ? 0 : ALL_ONES << (32 - n_bits % 32);
}

graphBits::graphBits(u32* ext_data, u32 n_bits, bool cleanout) {
    this->data = ext_data;  // since someone else gives me the data, they should
                            // have inited
    this->valid_len = n_bits;
    this->dlen = 1 + n_bits / 32;
    this->ext_ptr = true;
    if (cleanout) this->clear();
    this->pad_cover = n_bits % 32 == 0 ? 0 : ALL_ONES << (32 - n_bits % 32);
}

graphBits::graphBits(const graphBits& other) {
    this->valid_len = other.valid_len;
    this->dlen = other.dlen;
    this->data = new u32[other.dlen];
    this->ext_ptr = false;
    for (u32 i = 0; i < this->dlen; i++) this->data[i] = other.data[i];
    this->pad_cover = other.pad_cover;
}

graphBits::~graphBits() {
    if (!ext_ptr) {
        delete this->data;
        this->data = nullptr;
    }
}

void graphBits::set(u32 i) {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    this->data[i / 32] |= mask;
}

void graphBits::reset(u32 i) {
    // assert(i < this->valid_len);
    u32 mask = ~(MSB_32 >> (i % 32));
    this->data[i / 32] &= mask;
}

void graphBits::toggle(u32 i) {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    this->data[i / 32] ^= mask;
}

void graphBits::clear(u32 N) {
    u32 i = 0;
    if (N == 0 || N == this->valid_len || N / 32 == this->dlen) {
        for (i = 0; i < this->dlen; i++) this->data[i] = 0;
    } else {
        this->data[i] &= (ALL_ONES >> N % 32);
    }
}

u32 graphBits::count() {
    this->data[this->dlen - 1] &= this->pad_cover;
    u32 sum = 0;
    for (u32 i = 0; i < this->dlen; i++)
        sum += __builtin_popcount(this->data[i]);
    return sum;
}

bool graphBits::block_empty(u32 i) const { return (this->data[i / 32] == 0); }

bool graphBits::operator[](u32 i) const {
    // assert(i < this->valid_len);
    u32 mask = MSB_32 >> (i % 32);
    return (this->data[i / 32] & mask) != 0;
}

graphBits& graphBits::operator~() const {
    auto temp = new graphBits(this->valid_len);
    for (u32 i = 0; i < this->dlen; i++) {
        temp->data[i] = ~(this->data[i]);
    }
    // do I need to worry about what the padding bits are?
    // YES, for the popcnt stuff
    temp->data[this->dlen - 1] &= this->pad_cover;
    return *temp;
}

graphBits& graphBits::operator=(const graphBits& other) {
    if (this == &other) return *this;
    // is the below semantics okay for an equals operator?
    // I'm expecting a segfault when I chain too many =s in a set of expressions
    // What about x=x?

    // https://stackoverflow.com/questions/1734628/copy-constructor-and-operator-overload-in-c-is-a-common-function-possible
    // IS THIS SAME AS MOVE SEMANTICS??
    this->valid_len = other.valid_len;
    this->dlen = other.dlen;
    this->pad_cover = other.pad_cover;

    if (other.ext_ptr) {
        if (this->data == nullptr) {
            this->data = other.data;
            this->ext_ptr = true;
        } else {
            for (u32 i = 0; i < this->dlen; i++) this->data[i] = other.data[i];
        }
    } else {
        if (this->data == nullptr) this->data = new u32[this->dlen];
        for (u32 i = 0; i < this->dlen; i++) this->data[i] = other.data[i];
    }
    return *this;
}

graphBits& graphBits::operator&=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (u32 i = 0; i < this->dlen; i++) {
        this->data[i] &= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;

    return *this;
}

graphBits& graphBits::operator|=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (u32 i = 0; i < this->dlen; i++) {
        this->data[i] |= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;

    return *this;
}

graphBits& graphBits::operator^=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (u32 i = 0; i < this->dlen; i++) {
        this->data[i] ^= other.data[i];
    }
    this->data[this->dlen - 1] &= this->pad_cover;

    return *this;
}

graphBits& graphBits::operator-=(const graphBits& other) {
    // assert(this->valid_len == other.valid_len);
    for (u32 i = 0; i < this->dlen; i++) {
        this->data[i] &= ~(other.data[i]);
    }
    this->data[this->dlen - 1] &= this->pad_cover;

    return *this;
}

graphBits& graphBits::operator&(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    auto temp = new graphBits(this->valid_len);
    for (u32 i = 0; i < this->dlen; i++)
        temp->data[i] = this->data[i] & other.data[i];
    temp->data[temp->dlen - 1] &= temp->pad_cover;
    return *temp;
}

graphBits& graphBits::operator|(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    auto temp = new graphBits(this->valid_len);
    for (u32 i = 0; i < this->dlen; i++)
        temp->data[i] = this->data[i] | other.data[i];
    temp->data[temp->dlen - 1] &= temp->pad_cover;
    return *temp;
}

graphBits& graphBits::operator^(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    auto temp = new graphBits(this->valid_len);
    for (u32 i = 0; i < this->dlen; i++)
        temp->data[i] = this->data[i] ^ other.data[i];
    temp->data[temp->dlen - 1] &= temp->pad_cover;
    return *temp;
}

graphBits& graphBits::operator-(const graphBits& other) const {
    // assert(this->valid_len == other.valid_len);
    auto temp = new graphBits(this->valid_len);
    for (u32 i = 0; i < this->dlen; i++)
        temp->data[i] = this->data[i] & ~(other.data[i]);
    temp->data[temp->dlen - 1] &= temp->pad_cover;
    return *temp;
}

void graphBits::show() {
    for (u32 i = 0; i < this->valid_len; i++) std::cout << (*this)[i];
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

void graphBits::show(std::vector<u32> elements) {
    // assert(this->valid_len <= elements.size());
    for (u32 i = 0; i < this->valid_len; i++) {
        if ((*this)[i]) std::cout << elements[i] << " ";
    }
    // std::cout<<" ("<<this->count()<<" "<<this->pad_cover<<")\n";
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

void graphBits::show(u32* elements, u32 len) {
    // assert(this->valid_len <= len);
    for (u32 i = 0; i < this->valid_len; i++) {
        if ((*this)[i]) std::cout << elements[i] << " ";
    }
    // std::cout<<" ("<<this->count()<<" "<<this->pad_cover<<")\n";
    std::cout << " (" << this->count() << "/" << this->valid_len << ")\n";
}

std::vector<u32> graphBits::get_subset(std::vector<u32> elements) {
    // assert(this->valid_len <= elements.size());
    std::vector<u32> ans(this->count());
    for (u32 i = 0, ct = 0; i < this->valid_len; i++) {
        if ((*this)[i]) {
            ans[ct] = elements[i];
            ct++;
        }
    }

    return ans;
}

std::vector<u32> graphBits::get_subset(u32* elements, u32 len) {
    // assert(this->valid_len <= len);
    std::vector<u32> ans(this->count());
    for (u32 i = 0, ct = 0; i < this->valid_len; i++) {
        if ((*this)[i]) {
            ans[ct] = elements[i];
            ct++;
        }
    }

    return ans;
}
