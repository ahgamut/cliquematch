#ifndef CLIQUE_H
#define CLIQUE_H

#include <detail/graph.h>

namespace cliquematch
{
namespace detail
{
    class CliqueOperator
    {
       public:
        std::size_t request_size;
        CliqueOperator() { request_size = 0; }
        virtual ~CliqueOperator() = default;
        virtual std::size_t process_graph(graph&) = 0;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* CLIQUE_H */
