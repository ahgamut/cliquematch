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
        std::vector<u64> to_remove;       // vertices to remove from a candidate

        // the below variables have equivalent declared locally
        // in the member functions of RecursionDFS.
        u64 candidates_left, clique_size, clique_potential;
        u64 i, j, k, vert, ans;
        BFResult f;

       public:
        void process_vertex(graph&, u64);
        u64 process_graph(graph&);
        ~StackDFS() = default;
    };

    class RecursionDFS : public CliqueOperator
    {
       private:
        u64 start_vertex;
        double TIME_LIMIT;

       public:
        void process_vertex(graph&, u64);
        void search_vertex(graph&, u64, const graphBits&, graphBits&);
        u64 process_graph(graph&);
        RecursionDFS(u64 v, double t) : start_vertex(v), TIME_LIMIT(t){};
        ~RecursionDFS() = default;
    };

    class CliqueEnumerator : public CliqueOperator
    {
       private:
        std::vector<SearchState> states;
        std::vector<u64> to_remove;
        u64 candidates_left, clique_size, clique_potential;
        u64 j, k, vert, ans, cur;
        const u64 REQUIRED_SIZE;
        BFResult f;

       public:
        u64 process_graph(graph&);
        void process_vertex(graph&);
        bool load_vertex(graph&);
        CliqueEnumerator(u64 size) : cur(1), REQUIRED_SIZE(size){};
        CliqueEnumerator() = delete;
        ~CliqueEnumerator() = default;
    };

}  // namespace detail
}  // namespace cliquematch
#endif /* DFS_CLIQUE_H */
