Problem statement
------------
In this program, it use [MPI][src] to parallelize the computaion of integration of sin(x), which integrates function sin(x) over the range from 0 to 2 using N intervals, where N is an argument of the program.


[src]: https://computing.llnl.gov/tutorials/mpi/index.html 

Compile
------------

	mpicc -o integrate integrate.c -lm

Build
------------
    
    mpiexec -n 8 ./integrate number_of_intervals
