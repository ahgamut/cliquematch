#ifndef CLIQUE_H
#define CLIQUE_H

#include <core/graph.h>

class CliqueOperator
{
   protected:
    double TIME_LIMIT;

   public:
    CliqueOperator() = default;
    virtual ~CliqueOperator() = default;
    virtual std::size_t process_graph(graph&, std::size_t, double) = 0;
};

#endif /* CLIQUE_H */
