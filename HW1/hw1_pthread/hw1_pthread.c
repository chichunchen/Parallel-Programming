#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long long int unlong;

unlong number_in_circle = 0;
unlong points_per_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *runner() {
    unlong number_in_circle_in_thread = 0;

    unsigned int rand_state = rand();
    unlong i;
    for (i = 0; i < points_per_thread; i++) {
        double x = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;
        double y = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;

        if (x * x + y * y <= 1) {
            number_in_circle_in_thread++;
        }
    }

    pthread_mutex_lock(&mutex);
    number_in_circle += number_in_circle_in_thread;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, const char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: ./pi <number of tosses> <threads>\n");
        exit(1);
    }

    unlong number_of_tosses = atoll(argv[1]);
    int thread_count = atoi(argv[2]);
    points_per_thread = number_of_tosses / thread_count;

    /* Tried using clock, but it measures CPU time, not wall clock time,
     * so doesn't demonstrate improvement gained by threading */
    time_t start = time(NULL);

    srand((unsigned)time(NULL));

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int i;
    for (i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], &attr, runner, (void *) NULL);
    }

    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);

    double pi_estimate = (4. * (unlong)number_in_circle) / ((unlong)points_per_thread * thread_count); 

    printf("%f\n", pi_estimate);
    printf("Time: %d sec\n", (int)(time(NULL) - start));

    return 0;
}
