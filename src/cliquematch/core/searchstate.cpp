#include <core/searchstate.h>

SearchState::SearchState(const vertex& ver) {
    this->start_at = 0;
    this->res = graphBits(ver.bits);
    this->cand = ~(ver.bits);
}

SearchState::SearchState(const graphBits& prev_cand,
                         const graphBits& prev_res) {
    this->start_at = 0;
    this->res = graphBits(prev_res);
    this->cand = graphBits(prev_cand);
}
