# DEMaxSatSolver

DEMaxSATSolver is a incomplete solver for MaxSAT that combines a all-purposee genetic evolutionary algortihm like DE (Differential Evolution) with well-known local search heuristic for MaxSAT such GSAT and Random Walk. 

The solver is capable of solve weighted and unweighted instances of MaxSAT, but it can't handle PMS (Partial MaxSAT) instances (yet).

Author: Manuel Framil de Amorín

### Usage
Compile the program by typing
```
make
```
in the main folder.
Call the solver using:
```
./main path/to/wcnf [gens_logs]
```
where the first argument "path to wcnf" is the path to the MaxSAT instance to be solved, in DIMACS format, as defined in the MSE (https://maxsat-evaluations.github.io/2020/). You can also give the path to a directory with several WCNF files, and they'll be solved one by one. For each generation, are saved: the best score, the best individual of this gen, the mean of population and the current computing time. These values are saved in "gens_logs". If no path is given, the program will generate a new one.

Alternatively, you can set a timeout, and the solver will return a solution within that time:
```
timeout TIME ./main path/to/wcnf [gens_logs]
```

Following the output format of the MSE, the solver will return several 'o' lines, containing the best cost founded so far (that is, the sum of weights of the unsatisfied clauses) and one 'v' with the assigment of the best solution found.

### Results
The solver has been tested on 49 benchmarks submitted to the MSE'20, both weighted and unweighted, but all of them without hard clauses.

This solver has been developed in a very short time, it has achieved good results comparing with other solvers of the state of the art, tho. Here you can see a comparation between DEMaxSATSolver and other incomplete solvers presented on the MSE'20.

|                    | **Weight**      | **Unweight** |
|--------------------|-----------------|------------------:|
| satlike-cw         | **0.9633**      |         **0.9752**|
| StableReresolutor  | 0.8868          |            0.9730 |
| DEMaxSATsolver     | 0.8565          |            0.9300 |
| Loandra-2020       | 0.7519          |            0.9332 |
| TT-Open-WBO-Inc-20 | 0.7927          |            0.8802 |

In the folder "imgs" you can see several graphics comparing differents versions proposed.

### Contact
Contact me by email: m.framil.deamorin@udc.es
