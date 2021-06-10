# DEMaxSatSolver
This solver combine classical local search methods such as GSAT and Random Walk with a Differencial Evolution algortihm.

Author: Manuel Framil de Amorín

### Usage
Compile the program by typing
```
make
```
in the main folder.
Call the solver using:
```
./main path/to/wcnf
```
alternatively, you can set a timeout, and the solver will return a solution within that time:
```
timeout TIME ./main path/to/wcnf
```

### Results
See the comparations beetwen this solver and other incomplete solvers from the 2020 MaxSAT Evaluation in the folder imgs. You will find beetwen different modifications of the DEMaxSATSolver.
### Contact
Contact me by email: m.framil.deamorin@udc.es
