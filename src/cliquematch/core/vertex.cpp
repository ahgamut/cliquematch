#include <core/vertex.h>
#include <iostream>

short binary_find(u32* a, u32 N, u32 val, u32& loc) {
    // modified binary search, returns location by reference
    // return 1 if found, 0 if not found
    // returns -1 only if value is outside than the search space
    int beg = 0, end = N - 1, mid = (beg + end / 2);
    if (a[end] < val) {
        loc = end;
        return -1;
    }
    while (beg <= end) {
        if (a[mid] == val) {
            loc = mid;
            return 1;
        } else if (a[mid] < val)
            beg = mid + 1;
        else
            end = mid - 1;
        mid = (beg + end) / 2;
    }
    return 0;
}

vertex::vertex() {
    this->id = 0;
    this->N = 0;
    this->spos = 0;
    this->elo = 0;
    this->ebo = 0;
    this->mcs = 0;
}

vertex::vertex(u32 id, u32 N, u32 elo, u32 ebo) {
    this->id = id;
    this->N = N;
    this->elo = elo;
    this->ebo = ebo;
    this->mcs = 1;
    this->spos = 0;
}

void vertex::set_spos(u32* el_base, u32* eb_base) {
    short f = binary_find(el_base + this->elo, this->N, this->id, this->spos);
    this->bits = graphBits(eb_base + this->ebo, this->N);
    this->bits.set(this->spos);
}

void vertex::disp(u32* el_base) {
    if (this->N <= 1 || this->mcs <= 1) return;
    std::cout << "Vertex " << this->id << " has " << this->N << " edges\n";
    for (u32 i = 0; i < this->N; i++)
        std::cerr << *(el_base + this->elo + i) << " ";
    std::cout << "Current Clique: ";
    this->bits.show();
    this->bits.show(el_base + this->elo, this->N);
}

void vertex::clique_disp(u32* el_base) {
    this->bits.show(el_base + this->elo, this->N);
}

std::vector<u32> vertex::give_clique(u32* el_base) {
    return this->bits.get_subset(el_base + this->elo, N);
}
