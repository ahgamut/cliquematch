#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <detail/clique.h>

namespace cliquematch
{
namespace detail
{
    struct vtriple
    {
        std::size_t id, N, pos;
        void load(std::size_t _id, std::size_t _N, std::size_t _pos)
        {
            id = _id;
            N = _N;
            pos = _pos;
        }
        inline bool operator<(const vtriple& b) const { return (this->N < b.N); }
        inline bool operator>(const vtriple& b) const { return (this->N > b.N); }
    };

    class DegreeHeuristic : public CliqueOperator
    {
       private:
        std::vector<vtriple> neighbors;
        std::size_t clique_potential, candidates_left, cur_clique_size, cand_max;

       public:
        std::size_t process_graph(graph&);
        void process_vertex(graph&, std::size_t, graphBits&, graphBits&);
        DegreeHeuristic() = default;
        ~DegreeHeuristic() = default;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* HEURISTIC_H */
