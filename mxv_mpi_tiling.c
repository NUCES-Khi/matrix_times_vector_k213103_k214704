#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// Function to perform matrix-vector multiplication with tiling
void matrix_vector_multiply(double *matrix, double *vector, double *result, int local_rows, int local_cols, int rank) {
    int tile_size = 100; // Size of the tile
    double *tile_matrix = (double *)malloc(tile_size * local_cols * sizeof(double));
    double *tile_vector = (double *)malloc(tile_size * sizeof(double));
    double *tile_result = (double *)malloc(tile_size * sizeof(double));

    for (int i = 0; i < local_rows; i += tile_size) {
        for (int j = 0; j < local_cols; j += tile_size) {
            // Copy the tile from the global matrix and vector
            for (int ti = 0; ti < tile_size; ti++) {
                for (int tj = 0; tj < tile_size; tj++) {
                    if (i + ti < local_rows && j + tj < local_cols) {
                        tile_matrix[ti * tile_size + tj] = matrix[(i + ti) * local_cols + (j + tj)];
                    }
                }
            }
            for (int ti = 0; ti < tile_size; ti++) {
                if (i + ti < local_rows) {
                    tile_vector[ti] = vector[i + ti];
                }
            }

            // Perform the multiplication
            for (int ti = 0; ti < tile_size; ti++) {
                tile_result[ti] = 0.0;
                for (int tj = 0; tj < tile_size; tj++) {
                    tile_result[ti] += tile_matrix[ti * tile_size + tj] * tile_vector[tj];
                }
            }

            // Copy the result back to the global result vector
            for (int ti = 0; ti < tile_size; ti++) {
                if (i + ti < local_rows) {
                    result[i + ti] = tile_result[ti];
                }
            }
        }
    }

    free(tile_matrix);
    free(tile_vector);
    free(tile_result);
}

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = atoi(argv[1]) / size; // Calculate local_rows based on size and number of processes
    int local_cols = atoi(argv[1]); // Assuming square matrix for simplicity
    int global_rows = atoi(argv[1]);
    int global_cols = atoi(argv[1]);

    double *matrix = (double *)malloc(local_rows * local_cols * sizeof(double));
    double *vector = (double *)malloc(local_cols * sizeof(double));
    double *result = (double *)malloc(local_rows * sizeof(double));

    // Seed the random number generator
    srand(time(NULL));

    // Initialize matrix and vector
    for (int i = 0; i < local_rows; i++) 
    {
        for (int j = 0; j < local_cols; j++) 
        {
            matrix[i * local_cols + j] = rand() % 10; // Random value between 0 and 9
        }
    }
    for (int i = 0; i < local_cols; i++) 
    {
        vector[i] = rand() % 10; // Random value between 0 and 9
    }

    clock_t start = clock();
    matrix_vector_multiply(matrix, vector, result, local_rows, local_cols, rank);

    // Gather results from all processes
    double *global_result = NULL;
    if (rank == 0) 
    {
        global_result = (double *)malloc(global_rows * sizeof(double));
    }

    MPI_Gather(result, local_rows, MPI_DOUBLE, global_result, local_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for size %d: %f seconds\n", atoi(argv[1]), time_spent);

    if (rank == 0)
    {
        // Print the global result
        for (int i = 0; i < global_rows; i++) 
        {
            printf("Result[%d] = %f\n", i, global_result[i]);
        }
        free(global_result);
    }

    free(matrix);
    free(vector);
    free(result);

    MPI_Finalize();
    return 0;
}