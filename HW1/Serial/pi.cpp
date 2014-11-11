#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
using namespace std;

int thread_count;

double pi_estimate(long long int);
double pi_parallel_estimate(long long int);

int main(int argc, char const *argv[])
{
    clock_t start_t, end_t;
    long long int number_of_tosses = 0;

    cout << "Enter number of tosses: " ;
    cin >> number_of_tosses;
   
    start_t = clock();
    cout << "pi: " << pi_estimate(number_of_tosses) << endl;
    end_t = clock();

    float diff = ((float)end_t - (float)start_t);
    cout << "Calculating time is " << diff << " millisecond" << endl;

	return 0;
}

double pi_estimate(long long int number_of_tosses)
{
    long long int number_in_circle = 0;
    double x, y;
 
    for(int toss = 0; toss < number_of_tosses; toss++)
    {
       x = (2 * ((double) rand() / (RAND_MAX))) - 1;
       y = (2 * ((double) rand() / (RAND_MAX))) - 1;

       double distance_squared = x*x + y*y;
       if(distance_squared < 1)
           number_in_circle++;
    }

    return 4 * number_in_circle / ((double) number_of_tosses);
}

