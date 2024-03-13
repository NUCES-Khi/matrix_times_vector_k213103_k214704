#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 100 // Matrix and vector size (N rows and N cols)
#define TILE_SIZE 100 // Tile size for tiling

void matrix_vector_multiply(double *matrix, double *vector, double *result, int size) 
{
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < size; i += TILE_SIZE) {
        for (int j = 0; j < size; j += TILE_SIZE) {
            for (int ii = i; ii < i + TILE_SIZE; ++ii) {
                for (int jj = j; jj < j + TILE_SIZE; ++jj) {
                    result[ii] += matrix[ii * size + jj] * vector[jj];
                }
            }
        }
    }
}

int main(int argc, char **argv) 
{
    if (argc != 2) 
    {
        printf("Usage: %s <size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);

    double *matrix = (double *)malloc(size * size * sizeof(double));
    double *vector = (double *)malloc(size * sizeof(double));
    double *result = (double *)malloc(size * sizeof(double));

    // Seed the random number generator
    srand(time(NULL));

    // Initialize matrix and vector
    for (int i = 0; i < size; ++i) 
    {
        for (int j = 0; j < size; ++j) 
        {
            matrix[i * size + j] = rand() % 10; // Random value between 0 and 9
        }
        vector[i] = rand() % 10; // Random value between 0 and 9
        result[i] = 0.0;
    }

    // Measure time before computation
    double start = omp_get_wtime();  //clock() method not working for this

    // Perform matrix-vector multiplication
    matrix_vector_multiply(matrix, vector, result, size);
    #pragma omp barrier
    // Measure time after computation
    double end = omp_get_wtime();

    // Calculate and print the time taken
    printf("Time taken for size %d: %f seconds\n", size, end - start);

    // Free allocated memory
    free(matrix);
    free(vector);
    free(result);

    return 0;
}

