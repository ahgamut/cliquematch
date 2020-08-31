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
        SearchState(SearchState&& tmp)
            : start_at(tmp.start_at), id(tmp.id), cand(std::move(tmp.cand)),
              res(std::move(tmp.res)){};
        SearchState() = default;
        void refer_from(const graphBits&, const graphBits&, std::size_t);
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
