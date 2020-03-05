#include <pybind11/pybind11.h>
namespace py = pybind11;

/* TODO: Split into core and ext modules (exporting issue)
 * Is it possible to have core.so (with pygraph) and ext.so (with subclasses)
 * to which core is dynamically linked?
 */

