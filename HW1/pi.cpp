#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

double pi_estimate(long long int number_of_tosses)
{
    long long int number_in_circle = 0;
    double x, y;

    srand(time(0));     // use current time as seed for random generator
 
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

int main(int argc, char const *argv[])
{
    int number_of_tosses = 0;
    cin >> number_of_tosses;
    
    cout << pi_estimate(number_of_tosses);

	return 0;
}
