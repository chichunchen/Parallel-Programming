Problem statement
------------
In this program, it use [MPI][src] to parallelize the computaion of prime. 

It takes a long long int argument as an input, finds the largest prime number that is smaller than the input, and counts the prime numbers that are smaller than the input.

[src]: https://computing.llnl.gov/tutorials/mpi/index.html 

Compile
------------

	mpicc -o prime prime.c -lm

Build
------------
    
    mpiexec -n 8 ./prime number_of_limits
