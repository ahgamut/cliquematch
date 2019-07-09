#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_graph2(py::module&);
void init_A2Agraph(py::module&);
void init_A2Lgraph(py::module&);
void init_L2Agraph(py::module&);
void init_L2Lgraph(py::module&);

void init_IsoGraph(py::module&);  // deprecated
void init_psgraph(py::module&);   // deprecated

PYBIND11_MODULE(cm_base, m) {
    init_graph2(m);
    init_A2Agraph(m);
    init_A2Lgraph(m);
    init_L2Lgraph(m);
    init_L2Agraph(m);
}
