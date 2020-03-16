#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_pygraph(py::module&);
void init_A2Agraph(py::module&);
void init_A2Lgraph(py::module&);
void init_L2Agraph(py::module&);
void init_L2Lgraph(py::module&);
void init_Aligngraph(py::module&);

PYBIND11_MODULE(core, m)
{
    init_pygraph(m);
    init_A2Agraph(m);
    init_A2Lgraph(m);
    init_L2Lgraph(m);
    init_L2Agraph(m);
    init_Aligngraph(m);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
