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
        graphBits cand, res;

        SearchState(const vertex&);
        SearchState(const vertex& ver, u32* cand_ptr, u32* res_ptr)
            : start_at(0), id(ver.spos)
        {
            this->cand.refer_from(cand_ptr, ver.N);
            this->res.refer_from(res_ptr, ver.N);
            this->res.set(this->id);
        }
        SearchState(SearchState&& tmp)
            : start_at(tmp.start_at), id(tmp.id), cand(std::move(tmp.cand)),
              res(std::move(tmp.res)){};
        SearchState() = default;
        void refer_from(const graphBits&, const graphBits&, std::size_t);
        void refer_from(u32* cand_ptr, const graphBits& prev_cand,
                        const graphBits& prev_res, std::size_t id)
        {
            this->start_at = id + 1;
            this->id = id;
            this->cand.copy_from(prev_cand, cand_ptr);
            this->res.refer_from(prev_res);
            this->res.set(id);
        }
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
