#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void matrix_vector_multiply_mpi(int *matrix, int *vector, int *result, int size, int rank, int num_procs) {
    // Calculate the chunk size of matrix for each process
    int chunk_size = size / num_procs;

    // Allocate memory for the local matrix and result
    int *local_matrix = (int*)malloc(chunk_size * size * sizeof(int));
    int *local_result = (int*)malloc(chunk_size * sizeof(int));

    // Scatter the matrix among processes
    MPI_Scatter(matrix, chunk_size * size, MPI_INT, local_matrix, chunk_size * size, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast the vector to all processes
    MPI_Bcast(vector, size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform the matrix-vector multiplication locally
    for (int i = 0; i < chunk_size; i++) {
        local_result[i] = 0;
        for (int j = 0; j < size; j++) {
            local_result[i] += local_matrix[i*size + j] * vector[j];
        }
    }

    // Gather the local results from all processes
    MPI_Gather(local_result, chunk_size, MPI_INT, result, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Free the allocated memory
    free(local_matrix);
    free(local_result);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: %s <matrix_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int size = atoi(argv[1]);
    int *matrix = NULL, *vector = NULL, *result = NULL;

    if (rank == 0) {
        matrix = (int*)malloc(size * size * sizeof(int));
        vector = (int*)malloc(size * sizeof(int));
        result = (int*)malloc(size * sizeof(int));

        srand(time(NULL));
        for (int i = 0; i < size; i++) {
            vector[i] = rand() % 10; // Random values for the vector
            for (int j = 0; j < size; j++) {
                matrix[i*size + j] = rand() % 10; // Random values for the matrix
            }
        }
    }
    clock_t start = clock(); 
    // Broadcast the matrix size to all processes
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    matrix_vector_multiply_mpi(matrix, vector, result, size, rank, num_procs);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for size %d: %f seconds\n", size, time_spent);
    if (rank == 0) {
        free(matrix);
        free(vector);
        free(result);
    }

    MPI_Finalize();
    return 0;
}

