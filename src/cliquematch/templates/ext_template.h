#ifndef EXT_TEMPLATE_H
#define EXT_TEMPLATE_H

#include <core/pygraph.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <templates/relset.hpp>  //this contains only templates

namespace cliquematch
{
namespace ext
{
    using cliquematch::core::pygraph;
    using cliquematch::core::pygraphDeleter;
    namespace py = pybind11;

    template <typename List, typename Delta>
    Delta dummy_comparison(List& ll, std::size_t i, std::size_t j)
    {
        throw CM_ERROR("Can't have a distance function for an unknown type ");
        return (Delta)(0);
    }

    template <typename List1, typename List2>
    class SlowTemplate : public pygraph
    {
       public:
        bool build_edges(List1&, List2&);
        std::string showdata();
        pybind11::list get_correspondence(std::size_t len1, std::size_t len2);
        pybind11::list get_correspondence2(std::size_t len1, std::size_t len2,
                                           std::vector<std::size_t> clique);
    };

    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    class GraphTemplate : public SlowTemplate<List1, List2>
    {
       public:
        EpsType pts_epsilon;
        bool build_edges_condition_only(
            List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
            std::function<bool(List1&, std::size_t, std::size_t, List2&, std::size_t,
                               std::size_t)>
                cfunc);
        // overloads are defined as such because default arguments to pybind11
        // does not allow dummy_comparison<List, Delta>
        bool build_edges(List1& pts1, std::size_t pts1_len, List2& pts2,
                         std::size_t pts2_len,
                         std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                            std::size_t, std::size_t)>
                             cfunc,
                         std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
                         bool is_d1_symmetric,
                         std::function<Delta2(List2&, std::size_t, std::size_t)> d2,
                         bool is_d2_symmetric);
        bool build_edges(List1& pts1, std::size_t pts1_len, List2& pts2,
                         std::size_t pts2_len,
                         std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                            std::size_t, std::size_t)>
                             cfunc,
                         std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
                         bool is_d1_symmetric);
        bool build_edges(List1& pts1, std::size_t pts1_len, List2& pts2,
                         std::size_t pts2_len,
                         std::function<bool(List1&, std::size_t, std::size_t, List2&,
                                            std::size_t, std::size_t)>
                             cfunc);

        bool build_edges_metric_only(
            List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
            std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
            bool is_d1_symmetric,
            std::function<Delta2(List2&, std::size_t, std::size_t)> d2,
            bool is_d2_symmetric);
        bool build_edges_metric_only(
            List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
            std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
            bool is_d1_symmetric);
        bool build_edges_metric_only(List1& pts1, std::size_t pts1_len, List2& pts2,
                                     std::size_t pts2_len);
        std::string showdata();
    };

    /*
     * The below function takes two Lists, along with their subset relationships (ie
     * the relsets) and converts it into an undirected graph, by the following rule:
     *  s1[i] corresponds to s2[i'] and s1[j] corresponds to s2[j'] iff
     *  | d1(s1[i], s1[j]) - d2(s2[i'], s2[j']) | < epsilon
     *  where d1 is a distance metric to relate elements of s1
     *  and   d2 is a distance metric to relate elements of s2
     */
    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    std::vector<std::pair<std::size_t, std::size_t> > edges_from_relsets(
        std::size_t& nvert, std::size_t& nedges, const relset<List1, Delta1>&,
        const relset<List2, Delta2>&, const EpsType epsilon);

    /*
     * A more flexible version of the above function,
     * this one takes two Lists, along with their relsets, and converts it
     * to an undirected graph, by the following rule(s):
     *
     *  | d1(s1[i], s1[j]) - d2(s2[i'], s2[j']) | < epsilon
     *  where d1 is a distance metric to relate elements of s1
     *  and   d2 is a distance metric to relate elements of s2
     *
     * AND
     *
     * cfunc(i,j,i',j') => true
     * (cfunc internally accesses s1 and s2)
     *
     */
    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    std::vector<std::pair<std::size_t, std::size_t> > efr_condition(
        std::size_t& nvert, std::size_t& nedges, const relset<List1, Delta1>&,
        const relset<List2, Delta2>&, const EpsType epsilon,
        std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> cfunc);
}  // namespace ext
}  // namespace cliquematch
#endif /* EXT_TEMPLATE_H */

