#include <iostream>
#include <cblas.h>
#include <stdlib.h>
#include <mpi.h>
#include "src/util.h"
#include "src/naive.h"
#include "src/coded1.h"
#include "src/coded2.h"

#define NAIVE 0
#define CODED_1 1
#define CODED_2 2

#ifndef IMPLEMENTATION
#define IMPLEMENTION NAIVE
#endif

int main(void) {
    MPI_Init(NULL, NULL);

    int n_procs, proc_id, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Get_processor_name(processor_name, &name_len);

    //Necessary defines
#if !defined(N_ROWS) || !defined(N_COLS)
    cout << "N_ROWS or N_COLS not defined!" << endl;
    exit(0);
#endif

#if !defined(INPUT_VECTOR) || !defined(INPUT_MATRIX)
    cout << "INPUT_VECTOR or INPUT_MATRIX not defined!" << endl;
    exit(0);
#endif

    if (IMPLEMENTATION == NAIVE) {
	naive_matrix_vector_multiply(N_ROWS, N_COLS, INPUT_VECTOR, INPUT_MATRIX, RESULT_VECTOR);
    }
    else if (IMPLEMENTATION == CODED_1) {
	coded1_matrix_vector_multiply(N_ROWS, N_COLS, INPUT_VECTOR, INPUT_MATRIX, RESULT_VECTOR);
    }
    else if (IMPLEMENTATION == CODED_2) {
	coded2_matrix_vector_multiply(N_ROWS, N_COLS, INPUT_VECTOR, INPUT_MATRIX, RESULT_VECTOR);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
