Compile:
    mpicc -o prime prime.c -lm

Build:
    mpiexec -n 8 ./prime number_of_limits
