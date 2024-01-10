#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include <cstring>

using namespace std;

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
                        int **a_mat_ptr, int **b_mat_ptr){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_rank!=0)
        return;
    std::cin >> *n_ptr >> *m_ptr >> *l_ptr;

    *a_mat_ptr = new int[*n_ptr * *m_ptr];
    *b_mat_ptr = new int[*m_ptr * *l_ptr];

    for (int i = 0; i < *n_ptr; ++i) {
        for (int j = 0; j < *m_ptr; ++j) {
            std::cin >> (*a_mat_ptr)[i * *m_ptr + j];
        }
    }

    for (int i = 0; i < *m_ptr; ++i) {
        for (int j = 0; j < *l_ptr; ++j) {
            std::cin >> (*b_mat_ptr)[i * *l_ptr + j];
        }
    }
}

void matrix_multiply(const int n, const int m, const int l,
                     const int *a_mat, const int *b_mat){
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int n_len, m_len, l_len;
    if(world_rank==0){
        n_len=n;
        m_len=m;
        l_len=l;
    }
    MPI_Bcast(&n_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&l_len, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int block=n_len/world_size;
    int *local_a = new int[n_len*m_len];
    int *local_b = new int[m_len*l_len];
    int *local_result = new int[block*l_len];
    int size=block*l_len;

    int *result_mat = nullptr;
    if(world_rank == 0){
        result_mat = new int[n * l];
        std::memcpy(local_a, a_mat, sizeof(int) * n_len*m_len);
        std::memcpy(local_b, b_mat, sizeof(int) * m_len*l_len);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < l; ++j) {
                result_mat[i * l + j] = 0;
            }
        }
    }
    for(int i=0; i<size; ++i)
        local_result[i]=0;

    MPI_Bcast(local_a, n_len*m_len, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(local_b, m_len*l_len, MPI_INT, 0, MPI_COMM_WORLD);

    int begin=world_rank*block;
    for(int i=begin; i<begin+block; ++i){
        for (int k = 0; k < m_len; ++k) {
            for (int j = 0; j < l_len; ++j) {
                local_result[(i-begin) * l_len + j] += local_a[i * m_len + k] * local_b[k * l_len + j];
            }
        }        
    }


    MPI_Gather(local_result, block*l_len, MPI_INT, result_mat, block*l_len, MPI_INT, 0, MPI_COMM_WORLD);

    if(world_rank==0){
        if(n%world_size){
            int begin=world_size*block;
            for (int i = begin; i < n; ++i) {
                for (int k = 0; k < m; ++k) {
                    for (int j = 0; j < l; ++j) {
                        result_mat[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];
                    }
                }
            }            

        }
    }

    if(world_rank==0){
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < l; ++j) {
                printf("%d ",result_mat[i * l + j]);
            }
            printf("\n");
        }
    }
    delete[] local_a;
    delete[] local_b;
    delete[] local_result;
    if (world_rank == 0)
        delete[] result_mat;
}

void destruct_matrices(int *a_mat, int *b_mat){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_rank!=0)
        return;
    delete[] a_mat;
    delete[] b_mat;
}
