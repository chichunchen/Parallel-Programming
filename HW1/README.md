#Pi estiamtion with Monte Carlo method

## Usage 

    make
    ./pi <number of tosses> <threads>

## Problem Statement

Suppose we toss darts randomly at a square dartboard, whose bullseye is at the origin, and whose sides are two feet in length. Suppose also that there is a circle inscribed in the square dartboard. The radius of the circle is one foot, and its area is π square feet. If the points that are hit by the darts are uniformly distributed (and we always hit the square), then the number of darts that hit inside the circle should approximately satisfy the equation:

    number in circle / total number of tosses = π / 4

Since the ratio of the area of the circle to the area of the square is π/4. This is called a “Monte Carlo” method, since it uses randomness (the dart tosses). 


## Other

The main thread will read in the total number of tosses, thread total number and print the estimate. 

Use long long ints for the number of hits in the circle and the number of tosses, since both may have to be very large to get a reasonable estimate of π.
