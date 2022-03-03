# DEMaxSAT


DEMaxSAT is a memetic algorithm for solving the non-partial MaxSAT problem.
It combines the evolutionary algorithm of Differential Evolution with GSAT and RandomWalk, two MaxSAT-specific local search heuristics.
An implementation of the algorithm has been used to solve the benchmarks for non-partial MaxSAT included in the MaxSAT Evaluation 2021.
The performance of DEMaxSAT has reached results that are comparable, both in computing time and quality of the solutions, to the best solvers presented in MaxSAT Evaluation 2021, reaching the state of the art for non-partial problems.

Authors: Manuel Framil, Pedro Cabalar, Jose Santos

### Usage
Compile the program by typing
```
make
```
in the main folder.
Call the solver using:
```[text]
Usage: demaxsat [OPTION...] WCNF         Path to '.wcnf' file
DEMaxSAT solver is a MaxSAT solved which combines Differential Evolution with
ad-hoc maxsat heuristics, such as GSAT and RandomWalk.

  -c, --cr=CR                Crossover probability. Default=0.4f
  -f, --f=F                  Mutation probability. Default=0.6f
  -g, --gens=GENS            Max generations. No limit=-1. Default=-1
  -h, --hscope=H_SCOPE       Individuals affected by the Local Search heuristics [all|better_than_mean|best]. Default=all
  -l, --lss=LSS              Number of Local Search Steps. Is a percentage of the number of variables. Default=0.01
  -m, --maxlss=maxLSS        Max number of LSS on each call to local heuristics. No limit=-1.Default=100
  -p, --pop=POP              Population size. Default=100
  -r, --rw=RW                RandomWalk probability. GSAT prob=(1-RW). Default=0.5f
  -s, --seed=SEED            Random numbers seed. Randomly chosen = -1. Default=-1
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```
where the first argument "path to wcnf" is the path to the MaxSAT instance to be solved, in DIMACS format, as defined in the MSE (https://maxsat-evaluations.github.io/2021/). 

Following the output format of the MSE, the solver will return several 'o' lines, containing the best cost founded so far (that is, the sum of weights of the unsatisfied clauses) and one 'v' with the assigment of the best solution found.

### Results
The solver has been tested on non-partial benchmarks submitted to the MSE'21, both weighted and unweighted. The test were run within two timeouts 60s and 300s, like in the MSE.

The score for each solver has been calculated with the following equation:

<img src="https://latex.codecogs.com/svg.latex?\Large&space;\sum_{i \in \text{solved instances}} \frac{(\text{cost of the best known solution for i}) + 1}{(\text{cost of the solution found by the solver}) + 1}" 
title="Score equation" style="max-width: 50%;"/>

where the cost of a solution is the sum of the weights of the unsatisfied clauses. The following Table shows the average results in the set of benchmarks considered, for every solver in both 60s and 300s timeouts.

| **Solver**  | **60**	| **300** 	|
|:---------------------------|----------:|----------:|
| TT-Open-WBO-Inc-21 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;	| 	0.9698	|	0.9709	|
| satlike-c  		 		|	0.9703	|	0.9703	|
| satlike-ck     	 		| 	0.9703	|	0.9703 	|
| StableResolver     		| 	0.9406	|	0.9588 	|
| DEMaxSAT 			 		| 	0.9298	|	0.9433 	|
| Loandra-2020 		 		| 	0.8561	|	0.8713 	|
| inc-bmo-jb 		 		| 	0.8523	|	0.8712 	|
| inc-bmo-complete 	 		| 	0.8238	|	0.8438 	|




### Contact
Email: m.framil.deamorin@udc.es
