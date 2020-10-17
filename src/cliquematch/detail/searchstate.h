#ifndef SEARCHSTATE_H
#define SEARCHSTATE_H
#include <detail/vertex.h>

namespace cliquematch
{
namespace detail
{
    struct SearchState
    {
        std::size_t start_at, id;
        graphBits res, cand;

        SearchState() = default;
        SearchState(const vertex& ver, u32* res_ptr, u32* cand_ptr)
            : start_at(0), id(ver.spos), res(res_ptr, ver.N), cand(cand_ptr, ver.N)
        {
            this->res.set(this->id);
        }
        SearchState(SearchState&& tmp)
            : start_at(tmp.start_at), id(tmp.id), res(std::move(tmp.res)),
              cand(std::move(tmp.cand)){};

        void refer_from(u32* cand_ptr, const graphBits& prev_cand,
                        const graphBits& prev_res, std::size_t id)
        {
            this->start_at = id + 1;
            this->id = id;
            this->cand.copy_from(prev_cand, cand_ptr);
            this->res.refer_from(prev_res);
            this->res.set(this->id);
        }
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
