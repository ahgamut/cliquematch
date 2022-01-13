#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <detail/graph/clique.h>

namespace cliquematch
{
namespace detail
{
    class DegreeHeuristic : public CliqueOperator
    {
       private:
        struct vdata
        {
            u64 id, N, pos;
            void load(u64 _id, u64 _N, u64 _pos)
            {
                id = _id;
                N = _N;
                pos = _pos;
            }
            inline bool operator<(const vdata& b) const { return (this->N < b.N); }
            inline bool operator>(const vdata& b) const { return (this->N > b.N); }
        };

        std::vector<vdata> neighbors;
        u64 clique_potential, candidates_left, cur_clique_size, cand_max;

       public:
        u64 process_graph(graph&);
        void process_vertex(graph&, u64, graphBits&, graphBits&);
        DegreeHeuristic() = default;
        ~DegreeHeuristic() = default;
    };
}  // namespace detail
}  // namespace cliquematch
#endif /* HEURISTIC_H */
