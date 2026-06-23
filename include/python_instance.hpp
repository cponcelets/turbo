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
        return state->stats.get_mzn_statistics();
    }
};


