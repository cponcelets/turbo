import turbo_python
from pprint import pprint

def print_mzn_final_separator(stats):
    if stats['num_solutions'] > 0:
        if stats['exhaustive']:
            print("==========")
    else:
        assert stats['num_solutions'] == 0

        if stats['exhaustive']:
            print("=====UNSATISFIABLE=====")
        elif stats['optimization']:
            print("=====UNBOUNDED=====")
        else:
            print("=====UNKNOWN=====")


#print(dir(turbo_python))
solver = turbo_python.Turbo(["-a", "-t", "20000", "benchmarks/mini_example.fzn"])
solver.solve()

print_mzn_final_separator(solver.stats())
print("With:");
print(solver.output());

print("Stats:");
pprint(solver.stats());