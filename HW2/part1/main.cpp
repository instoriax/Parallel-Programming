#include <iostream>
#include <random>
#include <pthread.h>

using namespace std;

long long number_in_circle=0;
long long number_of_tosses;
int thread_count;
pthread_mutex_t mutex;

void* toss(void* rank) {
    long my_rank=(long)rank;
    long long tosses_per_thread=number_of_tosses/thread_count;
    unsigned int seed=time(NULL)+my_rank;
    long long local_number_in_circle = 0;

    for (long long toss=0; toss<tosses_per_thread; toss++) {
        double x = 2 * (double)rand_r(&seed) / (RAND_MAX) - 1;
        double y = 2 * (double)rand_r(&seed) / (RAND_MAX) - 1;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            local_number_in_circle++;
        }
    }

    pthread_mutex_lock(&mutex);
    number_in_circle += local_number_in_circle;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    number_of_tosses=atoll(argv[2]);
    thread_count=atoi(argv[1]);
    srand(time(NULL));
    vector<pthread_t> thread_handles(thread_count);
    pthread_mutex_init(&mutex, NULL);

    for (long thread=0; thread<thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, toss, (void*)thread);
    }

    for (long thread=0; thread<thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    pthread_mutex_destroy(&mutex);

    double pi_estimate = 4 * number_in_circle / (double)number_of_tosses;
    cout << pi_estimate << endl;

    return 0;
}
