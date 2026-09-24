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

/** The command line parser expects the program name as first argument, which is not given from Python. */
static const char* PROGRAM_NAME = "turbo_python";

static std::vector<std::string> with_program_name(const std::vector<std::string>& args) {
  std::vector<std::string> full_args{PROGRAM_NAME};
  full_args.insert(full_args.end(), args.begin(), args.end());
  return full_args;
}

PythonInstance::PythonInstance(const std::vector<std::string>& args):
config(parse_args(with_program_name(args))) {
  if(config.print_statistics) {
    printf("%%%%%%mzn-stat: command_line=\"");
    config.print_commandline(PROGRAM_NAME);
    printf("\"\n");
  }
  state = std::make_unique<CP<Itv>>(config);
}

/** Restore the signal handlers of Python when leaving `solve`, even with an exception. */
struct RestoreSignalGuard {
  ~RestoreSignalGuard() { restore_signal_ctrlc(); }
};

void PythonInstance::solve() {
  /** We start from a fresh state at each call: the solvers preprocess the problem again and accumulate their statistics into the state. */
  state = std::make_unique<CP<Itv>>(config);
  RestoreSignalGuard restore_signal_guard;
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
  fflush(stdout);
  fflush(stderr);
}
