#ifndef HEURISTIC_H
#define HEURISTIC_H

#include <core/clique.h>

struct vtriple
{
    std::size_t id, N, pos;
    inline bool operator<(const vtriple& b) const { return (this->N < b.N); }
    inline bool operator>(const vtriple& b) const { return (this->N > b.N); }
};

class DegreeHeuristic : public CliqueOperator
{
   private:
    std::vector<vtriple> neighbors;
    std::size_t mcs_potential, candidates_left, cur_clique_size, cand_max;

   public:
    std::size_t process_graph(graph&, std::size_t, double);
    void process_vertex(graph&, std::size_t, graphBits&, graphBits&);
    DegreeHeuristic() = default;
    ~DegreeHeuristic() = default;
};

#endif /* HEURISTIC_H */
