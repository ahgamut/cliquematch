#ifndef SEARCHSTATE_H
#define SEARCHSTATE_H

/* searchstate.h
 *
 * A SearchState is an element of the search stack used in an instance of
 * StackDFS or CliqueEnumerator. It contains minimum infomation to carry on the
 * clique search starting at a particular subtree, and needs the context in
 * StackDFS/CliqueEnumerator to be of any use. (this part is mostly
 * implicit/recomputed at every call in RecursionDFS).
 *
 * As each SearchState object contains two graphBits objects, SearchStates do
 * not manage memory, can only be moved, and require raw u32* pointers from the
 * CALLER.
 *
 */
#include <detail/vertex.h>

namespace cliquematch
{
namespace detail
{
    struct SearchState
    {
        const std::size_t id;  // index of the neighbor that's been added to the clique
        std::size_t start_at;  // index where the search should start
        graphBits res, cand;

        SearchState() : id(0){};
        SearchState(std::size_t id) : id(id), start_at(id + 1){};
        SearchState(const vertex& ver, u32* res_ptr, u32* cand_ptr)
            : id(ver.spos), start_at(0), res(res_ptr, ver.N), cand(cand_ptr, ver.N)
        {
            this->res.set(this->id);
        }
        SearchState(SearchState&& tmp)
            : id(tmp.id), start_at(tmp.start_at), res(std::move(tmp.res)),
              cand(std::move(tmp.cand)){};

        void refer_from(u32* cand_ptr, const graphBits& prev_cand,
                        const graphBits& prev_res)
        {
            // this is similar to the recursive call in RecursionDFS::search_vertex:
            // the cand is passed as a const reference and copied, but a reference to
            // res is used
            this->cand.copy_from(prev_cand, cand_ptr);
            this->res.refer_from(prev_res);
            this->res.set(this->id);
        }
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
