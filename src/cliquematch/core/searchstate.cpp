#include <core/searchstate.h>
namespace cliquematch
{
namespace detail
{
    SearchState::SearchState(const vertex& ver)
        : start_at(0), id(ver.spos), cand(ver.N), res(ver.N)
    {
        this->res.set(this->id);
    }

    void SearchState::refer_from(const graphBits& prev_cand, const graphBits& prev_res,
                                 std::size_t id)
    {
        this->start_at = id + 1;
        this->id = id;
        this->cand.copy_from(prev_cand);
        this->res.refer_from(prev_res);
        this->res.set(id);
    }
}  // namespace detail
}  // namespace cliquematch
