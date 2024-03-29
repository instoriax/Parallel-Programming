#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>

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
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_rank!=0)
        return;
    int *result_mat = new int[n * l];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < l; ++j) {
            result_mat[i * l + j] = 0;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < m; ++k) {
            for (int j = 0; j < l; ++j) {
                result_mat[i * l + j] += a_mat[i * m + k] * b_mat[k * l + j];
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < l; ++j) {
            printf("%d ",result_mat[i * l + j]);
        }
        printf("\n");
    }
}

void destruct_matrices(int *a_mat, int *b_mat){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_rank!=0)
        return;
    delete[] a_mat;
    delete[] b_mat;
}