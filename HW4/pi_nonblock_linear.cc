#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

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
    MPI_Request requests[world_size-1];
    int dest = 0;
    int tag = 0;
    long long number_in_circle = 0;
    std::vector<long long> counts(world_size-1,0);
    // TODO: MPI init

    if (world_rank > 0)
    {
        // TODO: MPI workers
        int my_rank=(int)world_rank;
        long long int tosses_per_process=tosses/world_size;
        long long local_number_in_circle = 0;
        local_number_in_circle=pi(tosses_per_process, my_rank);
        MPI_Send(&local_number_in_circle, 1, MPI_LONG_LONG, dest, tag, MPI_COMM_WORLD);

    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.

        // TODO: master
        
        for(int source=1; source<world_size; source++){
            MPI_Irecv(&counts[source-1], 1, MPI_LONG_LONG, source, tag, MPI_COMM_WORLD, &requests[source-1]);
        }

        int my_rank=0;
        long long int tosses_per_process=tosses/world_size;
        unsigned int seed=time(NULL);
        number_in_circle=pi(tosses_per_process, my_rank);

        MPI_Waitall(world_size-1, requests, MPI_STATUS_IGNORE);

        for(int i=1; i<world_size; i++){
            number_in_circle+=counts[i-1];
        }
    }

    if (world_rank == 0)
    {
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
