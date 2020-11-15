#ifndef DFS_CLIQUE_H
#define DFS_CLIQUE_H

/* dfs.h
 *
 * Contains the classes that perform depth-first search on a graph.
 *
 * StackDFS and RecursionDFS find only one maximum clique: both implement the
 * same algorithm.  StackDFS is faster: it doesn't check time, and is an
 * explicit version of the search using addtional memory to avoid steps.
 *
 * CliqueEnumerator finds all cliques of a given size. It is similar to
 * StackDFS but the search can be resumed to find the next clique.

 * RecursionDFS is easiest to understand. Bugs/updates for the algorithm are
 * first tested with RecursionDFS.
 *
 */

#include <detail/clique.h>
#include <detail/searchstate.h>

namespace cliquematch
{
namespace detail
{
    class StackDFS : public CliqueOperator
    {
       private:
        std::vector<SearchState> states;  // used like a stack with push_back/pop_back
        std::vector<std::size_t> to_remove;  // vertices to remove from a candidate

        // the below variables have equivalent declared locally
        // in the member functions of RecursionDFS.
        std::size_t candidates_left, clique_size, clique_potential;
        std::size_t i, j, k, vert, ans;
        short f;

       public:
        void process_vertex(graph&, std::size_t);
        std::size_t process_graph(graph&);
        ~StackDFS() = default;
    };

    class RecursionDFS : public CliqueOperator
    {
       private:
        std::size_t start_vertex;
        double TIME_LIMIT;

       public:
        void process_vertex(graph&, std::size_t);
        void search_vertex(graph&, std::size_t, const graphBits&, graphBits&);
        std::size_t process_graph(graph&);
        RecursionDFS(std::size_t v, double t) : start_vertex(v), TIME_LIMIT(t){};
        ~RecursionDFS() = default;
    };

    class CliqueEnumerator : public CliqueOperator
    {
       private:
        std::vector<SearchState> states;
        std::vector<std::size_t> to_remove;
        std::size_t candidates_left, clique_size, clique_potential;
        std::size_t i, j, k, vert, ans, cur;
        const std::size_t REQUIRED_SIZE;
        short f;

       public:
        std::size_t process_graph(graph&);
        void process_vertex(graph&);
        bool load_vertex(graph&);
        CliqueEnumerator(std::size_t size) : cur(1), REQUIRED_SIZE(size){};
        CliqueEnumerator() = delete;
        ~CliqueEnumerator() = default;
    };

}  // namespace detail
}  // namespace cliquematch
#endif /* DFS_CLIQUE_H */
