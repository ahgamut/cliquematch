#ifndef RELSET_H
#define RELSET_H

#include <templates/pair_dist.hpp>
#include <functional>
#include <set>
#include <vector>

template <typename List, typename Delta>
struct relset
{
    std::size_t N;
    std::vector<pair_dist<Delta> > dists;
    std::function<Delta(List&, std::size_t, std::size_t)> delfunc;
    // so delfunc will do the accessing of List,
    // and return the distance value
    bool symmetric;

    relset(std::size_t N, std::function<Delta(List&, std::size_t, std::size_t)> dfunc,
           bool symmetric = true);
    relset() { this->N = 0; }
    void fill_dists(List);
    void disp();
    /* TODO: try a disp() that can print the kth element of List without knowing
     * its type */
};

/*
 * The below function takes two Lists, along with their subset relationships (ie
 * the relsets) and converts it into an undirected graph, by the following rule:
 *  s1[i] corresponds to s2[i'] and s1[j] corresponds to s2[j'] iff
 *  | d1(s1[i], s1[j]) - d2(s2[i'], s2[j']) | < epsilon
 *  where d1 is a distance metric to relate elements of s1
 *  and   d2 is a distance metric to relate elements of s2
 */
template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
std::vector<std::set<std::size_t> > edges_from_relsets(std::size_t& nvert,
                                                       std::size_t& nedges,
                                                       relset<List1, Delta1>&,
                                                       relset<List2, Delta2>&,
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
template <typename List1, typename Delta1, typename List2, typename Delta2,
          typename EpsType>
std::vector<std::set<std::size_t> > efr_condition(
    std::size_t& nvert, std::size_t& nedges, relset<List1, Delta1>&,
    relset<List2, Delta2>&, const EpsType epsilon,
    std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> cfunc,
    bool use_cfunc_only);

#endif /* RELSET_H */

