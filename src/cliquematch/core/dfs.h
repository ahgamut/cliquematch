#ifndef DFS_CLIQUE_H
#define DFS_CLIQUE_H
#include <core/clique.h>
#include <core/searchstate.h>

class StackDFS : public CliqueOperator
{
   private:
    std::vector<SearchState> states;
    std::size_t candidates_left, mcs_potential;
    std::size_t i, j, k, vert, ans;
    short f;

   public:
    void process_vertex(graph&, std::size_t);
    std::size_t process_graph(graph&, std::size_t, double);
    ~StackDFS() = default;
};

class RecursionDFS : public CliqueOperator
{
   public:
    void process_vertex(graph&, std::size_t);
    void search_vertex(graph&, std::size_t, const graphBits&, const graphBits&);
    std::size_t process_graph(graph&, std::size_t, double);
    ~RecursionDFS() = default;
};

#endif /* DFS_CLIQUE_H */
