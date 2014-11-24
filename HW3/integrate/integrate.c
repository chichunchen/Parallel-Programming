#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define PI 3.1415926535

int main(int argc, char **argv) 
{
    int   ntasks,               /* total number of tasks in partitiion */
          rank,                 /* task identifier */
          FIRST = 0;            /* Rank of first task */

    MPI_Status status;

    long long   i,              /* loop varibale */ 
                num_intervals;

    double rect_width, area, sum, partial_sum, x_middle; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if(rank == FIRST) {
        sscanf(argv[1],"%llu",&num_intervals);
    }

    /* Sent num_intervals to all the other process from root process */
    MPI_Bcast(&num_intervals, 1, MPI_LONG_LONG_INT, FIRST, MPI_COMM_WORLD);

    rect_width = PI / num_intervals;

    partial_sum = 0;
    for(i = rank + 1; i < num_intervals + 1; i += ntasks) {
        /* find the middle of the interval on the X-axis. */ 
        x_middle = (i - 0.5) * rect_width;
        area = sin(x_middle) * rect_width; 
        partial_sum = partial_sum + area;
    } 

    MPI_Reduce(&partial_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, FIRST, MPI_COMM_WORLD);

    if(rank == FIRST) {
        printf("The total area is: %f\n", (float)sum);
    }

    MPI_Finalize();
}   
