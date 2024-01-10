#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

long long pi(long long int tosses_per_process, int my_rank){
    unsigned int seed=time(NULL)+my_rank;
    long long local_number_in_circle=0;
        for (long long toss=0; toss<tosses_per_process; toss++) {
            double x = 2 * (double)rand_r(&seed) / (RAND_MAX) - 1;
            double y = 2 * (double)rand_r(&seed) / (RAND_MAX) - 1;
            double distance_squared = x * x + y * y;
            if (distance_squared <= 1) {
                local_number_in_circle++;
            }
        }
    return local_number_in_circle;
}


int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long recv_values[world_size];
    int dest = 0;
    long long number_in_circle = 0;
    // TODO: MPI init

    // TODO: use MPI_Gather
    int my_rank=(int)world_rank;
    long long int tosses_per_process=tosses/world_size;
    long long local_number_in_circle = 0;
    local_number_in_circle=pi(tosses_per_process, my_rank);
    MPI_Gather(&local_number_in_circle, 1, MPI_LONG_LONG, &recv_values, 1, MPI_LONG_LONG, dest, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        for(int i=0; i<world_size; i++){
            number_in_circle+=recv_values[i];
        }
        // TODO: PI result
        double pi_result = 4 * number_in_circle / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
