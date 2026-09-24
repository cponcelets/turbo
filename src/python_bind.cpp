#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "python_instance.hpp"

using namespace pybind11;

PYBIND11_MODULE(turbo_python, m) {
	using namespace lala;
    
	m.doc() = "Turbo Solver python API";

    pybind11::class_<PythonInstance>(m, "Turbo")
    .def(pybind11::init<const std::vector<std::string>&>())
    .def("solve", &PythonInstance::solve)
    .def("best", &PythonInstance::best)
    .def("stats", &PythonInstance::stats);
}
