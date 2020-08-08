#ifndef SEARCHSTATE_H
#define SEARCHSTATE_H
#include <core/vertex.h>

namespace cliquematch
{
namespace detail
{
    struct SearchState
    {
        std::size_t start_at;
        graphBits cand, res;

        SearchState(const vertex&);
        SearchState(const graphBits&, const graphBits&);
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* SEARCHSTATE_H */
