#include <cm_base/wrappy/psgraph/psgraph.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;

//' Default constructor, use only for debugging purposes
psgraph::psgraph() {
    nvert = 0;
    nedges = 0;
    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100.0;
    pts_epsilon = 0.001;
    eps_choice = "constant";
    finished_heur = false;
    finished_all = false;
    current_vertex = 0;
}

//' Can load the point sets from a text file, stored as follows
//' <no of points in set 1>
//' <x1, y1>
//'  ...
//' <xM, yM>
//' <no of points in set 2>
//'  <x1, y1>
//'  ...
//'  <xN, yN>
//' Overloaded constructor from the base class, so `reader_choice` is actually a
// useless parameter
psgraph::psgraph(std::string filename) {
    std::cout << "Loading pointsets from file\n";
    std::string fname = filename;
    std::cerr << "Loading from: " << fname << "\n";

    if (true) {
        pts_reader(fname.c_str(), S1, S2, pts_epsilon);
        this->EDGES =
            edges_from_pointsets(nvert, nedges, S1, S2, 1, pts_epsilon);
    }

    this->G = graph(nvert, nedges, EDGES);

    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100.0;
    pts_epsilon = 0.001;
    finished_heur = false;
    finished_all = false;
    current_vertex = 0;
    eps_choice = "constant";
}

//' Constructor with two matrices.
//' Matrices need to be N rows x K columns (K == 2)
//' All points automatically considered as floats
psgraph::psgraph(Eigen::Ref<matrix> ps1, Eigen::Ref<matrix> ps2) {
    if (ps1.cols() > 2 || ps2.cols() > 2) {
        throw std::runtime_error(
            "(psgraph) Matrices can only have 2 dimensions!!\n");
    }
    // I am basically giving the root pointer of the numpy array to the pointset
    // (this might cause some problems later on)
    // TODO: Handle this with another set of Eigen::Refs, or subclass
    this->S1 = pointset(ps1.rows(), ps1);
    S1.fill_dists();

    this->S2 = pointset(ps2.rows(), ps2);
    S2.fill_dists();
}

//' Construct the edges of the correspondence graph
//' This step must be done once the points are loaded,
//' and every time the `epsilon` or `bounds_choice` values are changed
//' `epsilon` can be any positive real value
//' `bounds_choice` can be `"constant"`, `"ratio"`, or ""
void psgraph::build_edges() {
    if (eps_choice == "constant")
        EDGES = edges_from_pointsets(nvert, nedges, S1, S2, 1, pts_epsilon);
    else if (eps_choice == "ratio")
        EDGES = edges_from_pointsets(nvert, nedges, S1, S2, 2, pts_epsilon);
    else
        EDGES = edges_from_pointsets(nvert, nedges, S1, S2, 3, pts_epsilon);

    if (EDGES.data() == NULL || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges!!\n");
    }
    this->G = graph(nvert, nedges, EDGES);
}

//' Get a list containing correspondence information.
//' Returns a list containing:
//'  S1 : the subset of points from the first point set
//'  S2 : the subset of points from the second point set
py::list psgraph::get_correspondence() {
    if (!ans_found) find_max_clique();
    // so we're going with list of lists -> ndarray
    // send coefficients over, and let python take care of the Kabsch
    /* TODO: Write Eigen code for Kabsch  <12-03-19, gnv3> */
    py::list a1;
    py::list a2;

    auto S1mat = S1.get_mat();
    auto S2mat = S2.get_mat();
    u32 i, t1, t2;
    for (i = 0; i < ans_clique.size(); i++) {
        if (ans_clique[i] == 0) {
            throw std::runtime_error(
                "Sentinel Value (0) appeared in clique. Increase the time "
                "limit; if repeated, reset graph\n");
        }
        t1 = (ans_clique[i] - 1) / S2.N;
        t2 = (ans_clique[i] - 1) % S2.N;
        a1.append(return_doubleton<double>(S1mat(t1, 0), S1mat(t1, 1)));
        a2.append(return_doubleton<double>(S2mat(t2, 0), S2mat(t2, 1)));
    }

    py::list ans;
    py::array_t<double> ans1(a1);
    py::array_t<double> ans2(a2);

    ans.append(ans1);
    ans.append(ans2);

    return ans;
}

std::string psgraph::showdata() {
    std::stringstream ss;
    ss << "cliquematch.cm_base.psgraph object at " << this << "\n";
    ss << "Set 1 has " << this->S1.N << " points\n";
    ss << "Set 2 has " << this->S2.N << " points\n";
    ss << "epsilon = " << this->pts_epsilon << "\n";
    ss << "bounds_choice : " << this->eps_choice << "\n";

    if (this->nvert != 0) {
        ss << "Correspondence graph has " << this->nvert << " vertices, "
           << this->nedges << " edges\n";
        ss << "Search Parameters : \n";
        ss << "Size Limit : " << this->clique_lim
           << " Time limit: " << this->time_lim << "s\n";
        ss << "Using Heuristic : " << (this->use_heur ? "True" : "False");
        ss << " Using DFS: " << (this->use_dfs ? "True" : "False");
        ss << "\n";
        ss << "Search Completed? : " << this->finished_all << "\n";
    }
    return ss.str();
}

void psgraph::build_edges_with_filter(Eigen::Ref<matrix> controlpts,
                                      Eigen::Ref<matmask> img,
                                      double percentage) {
    EDGES = edges_filt_pointsets(nvert, nedges, S1, S2, controlpts, img,
                                 pts_epsilon, percentage);
    if (EDGES.data() == NULL || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges!!\n");
    }
    this->G = graph(nvert, nedges, EDGES);
}
