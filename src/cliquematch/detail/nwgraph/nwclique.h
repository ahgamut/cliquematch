#ifndef NWCLIQUE_H
#define NWCLIQUE_H

#include <detail/nwgraph/nwgraph.h>

namespace cliquematch
{
namespace detail
{
    class NWCliqueOperator
    {
       public:
        u64 request_size;

        NWCliqueOperator() : request_size(0){};
        virtual u64 process_graph(nwgraph&) = 0;
        virtual ~NWCliqueOperator() = default;
    };

    struct NWSearchState
    {
        const u64 id;  // index of the neighbor that's been added to the clique
        u64 start_at;  // index where the search should start
        double potential;
        graphBits res, cand;

        NWSearchState() : id(0), start_at(0), potential(0.0){};
        NWSearchState(u64 id, double potential)
            : id(id), start_at(id), potential(potential){};
        NWSearchState(const wvertex& ver, u64* res_ptr, u64* cand_ptr)
            : id(ver.spos), start_at(0), potential(0.0), res(res_ptr, ver.N),
              cand(cand_ptr, ver.N)
        {
            this->res.set(this->id);
        }
        NWSearchState(NWSearchState&& tmp)
            : id(tmp.id), start_at(tmp.start_at), potential(tmp.potential),
              res(std::move(tmp.res)), cand(std::move(tmp.cand)){};

        void refer_from(u64* cand_ptr, const graphBits& prev_cand,
                        const graphBits& prev_res)
        {
            // this is similar to the recursive call in RecursionDFS::search_vertex:
            // the cand is passed as a const reference and copied, but a reference to
            // res is used
            this->cand.copy_from(prev_cand, cand_ptr);
            this->res.refer_from(prev_res);
            this->res.set(this->id);
        }
    };

    class NWStackDFS : public NWCliqueOperator
    {
       private:
        std::vector<NWSearchState> states;  // used like a stack with push_back/pop_back
        std::vector<u64> to_remove;         // vertices to remove from a candidate
        std::vector<double> weights;

        double cand_potential, clique_weight, clique_potential;
        u64 candidates_left;
        u64 i, j, k, vert, ans;

       public:
        void process_vertex(nwgraph&, u64);
        u64 process_graph(nwgraph&);
        ~NWStackDFS() = default;
    };

    class NWCliqueEnumerator : public NWCliqueOperator
    {
       private:
        std::vector<NWSearchState> states;
        std::vector<u64> to_remove;
        std::vector<double> weights;

        double cand_potential, clique_weight, clique_potential;
        u64 candidates_left;
        u64 j, k, vert, ans, cur;
        const double REQUIRED_WEIGHT;

       public:
        u64 process_graph(nwgraph&);
        void process_vertex(nwgraph&);
        bool load_vertex(nwgraph&);
        NWCliqueEnumerator(double weight) : cur(1), REQUIRED_WEIGHT(weight){};
        NWCliqueEnumerator() = delete;
        ~NWCliqueEnumerator() = default;
    };

    class NWHeuristic : public NWCliqueOperator
    {
       private:
        struct vdata
        {
            u64 id, N, pos;
            double degree;
            void load(u64 _id, u64 _N, u64 _pos, double _deg)
            {
                id = _id;
                N = _N;
                pos = _pos;
                degree = _deg;
            }
            inline bool operator<(const vdata& b) const
            {
                return (this->degree < b.degree);
            }
            inline bool operator>(const vdata& b) const
            {
                return (this->degree > b.degree);
            }
        };

        std::vector<vdata> neighbors;
        u64 cand_max, candidates_left;
        double clique_potential, cur_clique_weight, cand_potential;

       public:
        u64 process_graph(nwgraph&);
        void process_vertex(nwgraph&, u64, graphBits&, graphBits&);
        NWHeuristic() = default;
        ~NWHeuristic() = default;
    };

}  // namespace detail
}  // namespace cliquematch

#endif /* NWCLIQUE_H */
