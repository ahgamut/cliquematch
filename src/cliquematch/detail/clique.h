#ifndef CLIQUE_H
#define CLIQUE_H

/* clique.h
 *
 * An abstract base class for every clique search operation.
 *
 * Every search operator has a request_size, which it can use to request the
 * graph object for memory.
 *
 * Every search operator processes a graph object, and returns the number of
 * vertices processed. Subclasses define additional functions to process
 * individual vertices, and have additional data members.
 *
 */
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
