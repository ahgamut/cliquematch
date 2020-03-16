#include <core/searchstate.h>

SearchState::SearchState(const vertex& ver)
{
    this->start_at = 0;
    this->res = graphBits(ver.N);
    this->res.set(ver.spos);
    this->cand = ~(this->res);
}

SearchState::SearchState(graphBits prev_cand, graphBits prev_res)
{
    this->start_at = 0;
    this->res = graphBits(prev_res);
    this->cand = graphBits(prev_cand);
}
