# Conjugate gradient method with OpenMP


Conjugate gradient method is an algorithm for the numerical solution of particular systems of linear equations. It is often used to solve partial differential equations, or applied on some optimization problems. 

This program use openmp to speedup.

## Files:

cg.c : main functions, the implementaion of conjugate gradient method.
globals.h : some data defination.
common : functions for verification and time caculation.
bin : excutable output directory.
Makefile, make.common : make systems.


## Build up:

    make DATASIZE=[LARGE|MEDIUMN|SMALL]
(If not assign DATASIZE, default will be mediumn)
    Please do make clean if you want to change datasize.


## Check correctness:

    Main function contains verification information, it will show VERIFICATION SUCCESSFUL/FAILED to indicate the correctness of the program.


## Environment:

gcc version 4.6.3 (Ubuntu/Linaro 4.6.3-1ubuntu5)

    This program use openmp to speedup, and should run on compiler which support openmp library (Apple's compiler llvm is not supporting anymore).
