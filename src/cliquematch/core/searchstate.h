#ifndef SEARCHSTATE_H
#define SEARCHSTATE_H
#include <core/vertex.h>

namespace cliquematch
{
namespace detail
{
    struct SearchState
    {
        std::size_t start_at, id;
        graphBits cand, res;

        SearchState(const vertex&);
        SearchState() = default;
        void refer_from(const graphBits&, const graphBits&, std::size_t);
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
