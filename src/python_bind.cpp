#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "incremental_turbo.hpp"

namespace py = pybind11;

int solve_wrapper(const std::vector<std::string>& args) {
    std::vector<char*> c_args;

    for (const auto& s : args) {
        c_args.push_back(const_cast<char*>(s.c_str()));
    }

    int result = solve((int)c_args.size(), c_args.data());
    cudaDeviceSynchronize();
    fflush(stdout);
    fflush(stderr);

    return result;
}


PYBIND11_MODULE(turbo_python, m) {
	m.doc() = "Turbo Solver python API";
	m.def("solve", &solve_wrapper, "Turbo solve", py::arg("args"));
}
