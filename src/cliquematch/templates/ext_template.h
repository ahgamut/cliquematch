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
    Delta dummy_comparison(const List& ll, const std::size_t i, const std::size_t j)
    {
        throw CM_ERROR("Can't have a distance function for an unknown type ");
        return (Delta)(0);
    }

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
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> edges_from_relsets(
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
    std::pair<std::vector<std::size_t>, std::vector<std::size_t>> efr_condition(
        std::size_t& nvert, std::size_t& nedges, const relset<List1, Delta1>&,
        const relset<List2, Delta2>&, const EpsType epsilon,
        const std::function<bool(const std::size_t, const std::size_t,
                                 const std::size_t, const std::size_t)>
            cfunc);

    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    bool build_edges_condition_only(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len,
        const std::function<bool(const List1&, const std::size_t, const std::size_t,
                                 const List2&, const std::size_t, const std::size_t)>
            cfunc);
    // overloads are defined as such because default arguments to pybind11
    // does not allow dummy_comparison<List, Delta>
    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    bool build_edges(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len, const EpsType epsilon,
        const std::function<bool(const List1&, const std::size_t, const std::size_t,
                                 const List2&, const std::size_t, const std::size_t)>
            cfunc,
        const std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
            d1 = dummy_comparison<List1, Delta1>,
        const bool is_d1_symmetric = true,
        const std::function<Delta2(const List2&, const std::size_t, const std::size_t)>
            d2 = dummy_comparison<List2, Delta2>,
        const bool is_d2_symmetric = true);

    template <typename List1, typename List2, typename Delta1 = double,
              typename Delta2 = Delta1, typename EpsType = Delta1>
    bool build_edges_metric_only(
        pygraph& pg, const List1& pts1, const std::size_t pts1_len, const List2& pts2,
        const std::size_t pts2_len, const EpsType epsilon,
        const std::function<Delta1(const List1&, const std::size_t, const std::size_t)>
            d1 = dummy_comparison<List1, Delta1>,
        const bool is_d1_symmetric = true,
        const std::function<Delta2(const List2&, const std::size_t, const std::size_t)>
            d2 = dummy_comparison<List2, Delta2>,
        const bool is_d2_symmetric = true);
}  // namespace ext

}  // namespace cliquematch
#endif /* EXT_TEMPLATE_H */

