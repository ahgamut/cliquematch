#include <wrappy/egraph/egraph.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace py = pybind11;

Egraph::Egraph() {
    nvert = 0;
    nedges = 0;
    use_heur = false;
    use_dfs = true;
    clique_lim = 32;
    time_lim = 100;
    pts_epsilon = 0.001;
    finished_heur = false;
    finished_all = false;
    current_vertex = 0;
}

Egraph::Egraph(Eigen::Ref<matrix> pts1, u32 pts1_len, Eigen::Ref<matrix> pts2,
               u32 pts2_len,
               std::function<double(Eigen::Ref<matrix>&, u32, u32)> d1,
               std::function<double(Eigen::Ref<matrix>&, u32, u32)> d2,
               bool is_d1_symmetric, bool is_d2_symmetric) {
    this->ps1 = pset(pts1_len, d1, is_d1_symmetric);
    this->ps2 = pset(pts2_len, d2, is_d2_symmetric);

    this->ps1.fill_dists(pts1);
    this->ps2.fill_dists(pts2);
}

bool Egraph::build_edges() {
    EDGES = edges_from_relsets(nvert, nedges, this->ps1, this->ps2,
                               this->pts_epsilon);

    if (EDGES.data() == nullptr || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges");
    }

    this->G = graph(nvert, nedges, EDGES);
    return true;
}

bool Egraph::build_edges_with_condition(
    Eigen::Ref<matrix>& pts1, Eigen::Ref<matrix>& pts2,
    std::function<bool(Eigen::Ref<matrix>&, u32, u32, Eigen::Ref<matrix>&, u32,
                       u32)>
        cfunc,
    bool use_cfunc_only) {
    auto cfwrap = [&pts1, &pts2, &cfunc](u32 i, u32 j, u32 i2, u32 j2) -> bool {
        return cfunc(pts1, i, j, pts2, i2, j2);
    };
    EDGES = efr_condition(nvert, nedges, this->ps1, this->ps2,
                          this->pts_epsilon, cfwrap, use_cfunc_only);
    if (EDGES.data() == nullptr || EDGES.size() == 0) {
        throw std::runtime_error("Could not extract edges");
    }
    this->G = graph(nvert, nedges, EDGES);
    return true;
}

py::list Egraph::get_correspondence() {
    if (!ans_found) find_max_clique();

    //    auto S1mat = this->attr("__dict__")["S1"].unchecked<2>();
    //    auto S2mat = this->attr("__dict__")["S2"].unchecked<2>();

    py::list a1;
    py::list a2;

    u32 i, t1, t2;
    for (i = 0; i < ans_clique.size(); i++) {
        if (ans_clique[i] == 0) {
            throw std::runtime_error(
                "Sentinel Value (0) appeared in clique. Increase the time "
                "limit; if repeated, reset graph\n");
        }
        t1 = (ans_clique[i] - 1) / ps2.N;
        t2 = (ans_clique[i] - 1) % ps2.N;
        a1.append(t1);
        a2.append(t2);
    }
    py::list ans;

    ans.append(a1);
    ans.append(a2);

    return ans;
}

std::string Egraph::showdata(bool fully) {
    std::stringstream ss;
    ss << "cliquematch.cm_base.Egraph object at " << this << "\n";
    ss << "epsilon = " << this->pts_epsilon << "\n";

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
        if (fully) {
            ss << "Vertex data:\n";
            this->G.disp(ss);
        }
    }
    return ss.str();
}
