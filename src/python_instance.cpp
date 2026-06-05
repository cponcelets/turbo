// Copyright 2022 Pierre Talbot
#include <iostream>
#include "cpu_solving.hpp"

#ifdef __CUDACC__
  #include <cuda_runtime.h>
#endif

/** IPC abstract domain only works when REDUCE_PTX_SIZE is activated due to numerous nested recursive calls. */
#ifdef TURBO_IPC_ABSTRACT_DOMAIN
#ifndef REDUCE_PTX_SIZE
#define DISABLE_FULL_GPU_SOLVING
#endif
#endif

#ifndef DISABLE_FULL_GPU_SOLVING
#include "gpu_dive_and_solve.hpp"
#endif

#include "hybrid_dive_and_solve.hpp"
#include "barebones_dive_and_solve.hpp"

#include "python_instance.hpp"

PythonInstance::PythonInstance(const std::vector<std::string>& args):
config(parse_args(args)) {
  if(config.print_statistics) {
    printf("%%%%%%mzn-stat: command_line=\"");
    config.print_commandline(args.at(0).c_str());
    printf("\"\n");
  }
  state = std::make_unique<CP<Itv>>(config);
}

void PythonInstance::solve() {
  if(!state) {
    throw std::runtime_error("Call init() first.");
  } else {
    if(config.arch == Arch::CPU) {
      cpu_solve(*state);
    }
#ifndef DISABLE_FULL_GPU_SOLVING
    else if(config.arch == Arch::GPU) {
      gpu_dive_and_solve(*state);
    }
#endif
    else if(config.arch == Arch::BAREBONES) {
      if(config.print_intermediate_solutions) {
        printf("%% WARNING: -arch barebones is incompatible with -i and -a (it cannot print intermediate solutions).\n");
      }
      barebones::barebones_dive_and_solve(*state);
    }
#ifndef DISABLE_HYBRID_GPU_SOLVING
    else if(config.arch == Arch::HYBRID) {
      hybrid_dive_and_solve(*state);
    }
#endif
  }
  fflush(stdout);
  fflush(stderr);
}
