#include <relset/eigrel.h>
#include <cmath>

template <typename DerivedMat>
double eucdist0(const Eigen::MatrixBase<DerivedMat>& p,
                const Eigen::MatrixBase<DerivedMat>& q) {
    // auto t = (p - q).array();
    // double answer = t.pow(2).sum().sqrt();
    return (p - q).norm();
}

double eucdist(Eigen::Ref<matrix> mat, u32 i, u32 j) {
    // double answer = 0;
    return eucdist0(mat.row(i), mat.row(j));
    // answer += (mat(i, 0) - mat(j, 0)) * (mat(i, 0) - mat(j, 0));
    // answer += (mat(i, 1) - mat(j, 1)) * (mat(i, 1) - mat(j, 1));
    // answer = std::sqrt(answer);
    // return answer;
}

template struct relset<Eigen::Ref<matrix>, double>;

template std::vector<std::set<u32> >
edges_from_relsets<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double,
                   double>(u32&, u32&, pset&, pset&, const double);

template std::vector<std::set<u32> >
efr_condition<Eigen::Ref<matrix>, double, Eigen::Ref<matrix>, double, double>(
    u32&, u32&, pset&, pset&, const double,
    std::function<bool(u32, u32, u32, u32)>, bool);
