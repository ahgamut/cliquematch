#ifndef EXT_TEMPLATE_H
#define EXT_TEMPLATE_H

#include <core/pygraph.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <templates/relset.hpp>  //this contains only templates

template <typename List, typename Delta>
Delta dummy_comparison(List& ll, std::size_t i, std::size_t j)
{
    throw CM_ERROR("Can't have a distance function for an unknown type ");
    return (Delta)(0);
}

template <typename List1, typename Delta1, typename List2, typename Delta2 = Delta1,
          typename EpsType = Delta1>
class GraphTemplate : public pygraph
{
   private:
    relset<List1, Delta1> ps1;
    relset<List2, Delta2> ps2;

   public:
    EpsType pts_epsilon;

    GraphTemplate();
    GraphTemplate(List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
                  std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
                  bool is_d1_symmetric,
                  std::function<Delta2(List2&, std::size_t, std::size_t)> d2,
                  bool is_d2_symmetric);

    GraphTemplate(List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len,
                  std::function<Delta1(List1&, std::size_t, std::size_t)> d1,
                  bool is_d1_symmetric = true)
        : GraphTemplate(pts1, pts1_len, pts2, pts2_len, d1, is_d1_symmetric,
                        dummy_comparison<List2, Delta2>, true)
    {
    }

    GraphTemplate(List1& pts1, std::size_t pts1_len, List2& pts2, std::size_t pts2_len)
        : GraphTemplate(pts1, pts1_len, pts2, pts2_len, dummy_comparison<List1, Delta1>,
                        true, dummy_comparison<List2, Delta2>, true)
    {
    }

    bool build_edges(List1& pts1, List2& pts2);
    bool build_edges_with_condition(
        List1& pts1, List2& pts2,
        std::function<bool(List1&, std::size_t, std::size_t, List2&, std::size_t,
                           std::size_t)>
            cfunc,
        bool use_cfunc_only = false);

    std::string showdata();
    pybind11::list get_correspondence();
    pybind11::list get_correspondence2(std::vector<std::size_t> clique);
};

/*
 * The below function takes two Lists, along with their subset relationships (ie
 * the relsets) and converts it into an undirected graph, by the following rule:
 *  s1[i] corresponds to s2[i'] and s1[j] corresponds to s2[j'] iff
 *  | d1(s1[i], s1[j]) - d2(s2[i'], s2[j']) | < epsilon
 *  where d1 is a distance metric to relate elements of s1
 *  and   d2 is a distance metric to relate elements of s2
 */
template <typename List1, typename Delta1, typename List2, typename Delta2 = Delta1,
          typename EpsType = Delta1>
std::vector<std::set<std::size_t> > edges_from_relsets(std::size_t& nvert,
                                                       std::size_t& nedges,
                                                       const relset<List1, Delta1>&,
                                                       const relset<List2, Delta2>&,
                                                       const EpsType epsilon);

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
 * setting the parameter use_cfunc_only as true will
 * skip the epsilon check,
 * causing a time complexity of N^4.
 */
template <typename List1, typename Delta1, typename List2, typename Delta2 = Delta1,
          typename EpsType = Delta1>
std::vector<std::set<std::size_t> > efr_condition(
    std::size_t& nvert, std::size_t& nedges, const relset<List1, Delta1>&,
    const relset<List2, Delta2>&, const EpsType epsilon,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> cfunc,
    bool use_cfunc_only);

#endif /* EXT_TEMPLATE_H */

