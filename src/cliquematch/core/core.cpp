#include <pybind11/pybind11.h>

namespace cliquematch
{
namespace py = pybind11;
void init_pygraph(py::module&);
void init_iterators(py::module&);
void init_A2Agraph(py::module&);
void init_A2Lgraph(py::module&);
void init_L2Agraph(py::module&);
void init_L2Lgraph(py::module&);
void init_Aligngraph(py::module&);
void init_Isograph(py::module&);
}  // namespace cliquematch

PYBIND11_MODULE(core, m)
{
    using namespace pybind11;
    namespace cm = cliquematch;
    cm::init_pygraph(m);
    cm::init_iterators(m);
    cm::init_A2Agraph(m);
    cm::init_A2Lgraph(m);
    cm::init_L2Agraph(m);
    cm::init_L2Lgraph(m);
    cm::init_Aligngraph(m);
    cm::init_Isograph(m);
}
