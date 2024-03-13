#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matrix_vector_multiply(int *matrix, int *vector, int *result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = 0;
        for (int j = 0; j < size; j++) {
            result[i] += matrix[i*size + j] * vector[j];
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }
    //argv[1], the 1st command line argument, is size for matrix and vectors
    int size = atoi(argv[1]);
    int *matrix = (int*)malloc(size * size * sizeof(int));
    int *vector = (int*)malloc(size * sizeof(int));
    int *result = (int*)malloc(size * sizeof(int));

    srand(time(NULL));
    //Gonna Randomize all the values inserted into the vector
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 10; // Random values for the vector
        for (int j = 0; j < size; j++) {
            matrix[i*size + j] = rand() % 10; // Random values for the matrix
        }
    }

    clock_t start = clock();
    matrix_vector_multiply(matrix, vector, result, size);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for size %d: %f seconds\n", size, time_spent);

    free(matrix);
    free(vector);
    free(result);
    return 0;
}
