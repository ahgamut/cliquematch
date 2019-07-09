#ifndef PSGRAPH_H
#define PSGRAPH_H

#include <cm_base/pointset/pointset.h>
#include <cm_base/wrappy/graph2/graph2.h>
#include <pybind11/eigen.h>

//' Data structure for storing the pairs of point-sets and their correspondence
// graph
// Currently adding new points would mean the constructor has to be
// called again
struct psgraph : public graph2 {
    pointset S1, S2;
    double pts_epsilon;
    std::string eps_choice;

    psgraph();
    psgraph(std::string filename);
    psgraph(Eigen::Ref<matrix> pts1, Eigen::Ref<matrix> pts2);
    void build_edges();
    pybind11::list get_correspondence();
    std::string showdata();

    void build_edges_with_filter(Eigen::Ref<matrix> controlpts,
                                 Eigen::Ref<matmask> img,
                                 double percentage = 0.8);
};

#endif /* PSGRAPH_H */
