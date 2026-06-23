#include "common_solving.hpp"
#include "statistics.hpp"

using Itv = Interval<ZLB<bound_value_type, battery::local_memory>>;
using StatValue =
    std::variant<bool, size_t, int, long long, double, std::string>;

struct PythonInstance {
    private:
        Configuration<battery::standard_allocator> config;
        //Permanent state to access in python
        std::unique_ptr<CP<Itv>> state;

        using bstring = battery::string<battery::standard_allocator>;
        template<class T> using bvector = battery::vector<T, battery::standard_allocator>;

    public:
    PythonInstance(const std::vector<std::string>& args);

    //Launch a solve 
    void solve();
    
    const std::vector<std::tuple<std::string, std::string>> best() const {
        if(state == nullptr) {
            throw std::runtime_error("State not initialized");
        }
        std::vector<std::tuple<std::string, std::string>> result;
        
        const auto& vars = state->solver_output.get_output_vars();
        for(int i = 0; i < vars.size(); ++i) {
            result.emplace_back(
                vars[i].data(),
                state->solver_output.var_to_string(vars[i], state->env, *state->best, *state->simplifier)
            );
        }
        return result;
    }

    std::unordered_map<std::string, StatValue> stats() const {
        if(state == nullptr) {
            throw std::runtime_error("State not initialized");
        }
        return get_mzn_statistics();
    }

    CUDA std::unordered_map<std::string, StatValue> get_mzn_statistics(int verbose = 0) const {
    return {
      {"num_blocks", state->stats.num_blocks},
      {"nodes", state->stats.nodes},
      {"failures", state->stats.fails},
      {"variables", state->stats.variables},
      {"propagators", state->stats.constraints},
      {"optimization", state->stats.optimization},
      {"peakDepth", state->stats.depth_max},
      {"initTime", state->stats.to_sec(state->stats.timers.time_of(Timer::PREPROCESSING))},
      {"solveTime", state->stats.to_sec(state->stats.timers.time_of(Timer::OVERALL))},
      {"num_solutions", state->stats.solutions},
      {"exhaustive", state->stats.exhaustive},
      {"eps_num_subproblems", state->stats.eps_num_subproblems},
      {"eps_solved_subproblems", state->stats.eps_solved_subproblems},
      {"eps_skipped_subproblems", state->stats.eps_skipped_subproblems},
      {"num_blocks_done", state->stats.num_blocks_done},
      {"fixpoint_iterations", state->stats.fixpoint_iterations},
      {"num_deductions", state->stats.num_deductions},

      // Timing statistics
      {"cumulative_time_block_sec", state->stats.to_sec(state->stats.cumulative_time_block)},
      {"deductions_per_block_second", state->stats.num_deductions / state->stats.num_blocks / state->stats.to_sec(state->stats.cumulative_time_block)},
      {"solve_time", state->stats.to_sec(state->stats.timers.time_of(Timer::OVERALL) / state->stats.num_blocks)},
      {"search_time", state->stats.to_sec(state->stats.timers.time_of(Timer::SEARCH) / state->stats.num_blocks)},
      // print_block_timing_stat("split_time", Timer::SPLIT);
      // print_block_timing_stat("push_time", Timer::PUSH);
      // print_block_timing_stat("pop_time", Timer::POP);
      {"fixpoint_time", state->stats.to_sec(state->stats.timers.time_of(Timer::FIXPOINT) / state->stats.num_blocks)},
      {"transfer_cpu2gpu_time", state->stats.to_sec(state->stats.timers.time_of(Timer::TRANSFER_CPU2GPU) / state->stats.num_blocks)},
      {"transfer_gpu2cpu_time", state->stats.to_sec(state->stats.timers.time_of(Timer::TRANSFER_GPU2CPU) / state->stats.num_blocks)},
      {"select_fp_functions_time", state->stats.to_sec(state->stats.timers.time_of(Timer::SELECT_FP_FUNCTIONS) / state->stats.num_blocks)},
      {"wait_cpu_time", state->stats.to_sec(state->stats.timers.time_of(Timer::WAIT_CPU) / state->stats.num_blocks)},
      {"dive_time", state->stats.to_sec(state->stats.timers.time_of(Timer::DIVE) / state->stats.num_blocks)},
      {"best_obj_time", state->stats.to_sec(state->stats.timers.time_of(Timer::LATEST_BEST_OBJ_FOUND))},
      {"first_block_idle_time", state->stats.to_sec(state->stats.timers.time_of(Timer::FIRST_BLOCK_IDLE))}
    };
  }

  
};


