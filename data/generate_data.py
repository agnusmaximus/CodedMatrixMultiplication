from __future__ import print_function
from time import sleep
import sys
import random
import os
import copy
from generate_data_helpers import *

if len(sys.argv) != 7:
    print("Usage: python generate_partitioned_matrix.py n_procs is_coded n_cols n_rows output_dir should_output_result")
    sys.exit(0)

random.seed(0)
n_procs, is_coded, n_cols, n_rows, output_dir, should_calculate_answer  = [x for x in sys.argv[1:]]
n_procs, is_coded, n_cols, n_rows, should_calculate_answer = int(n_procs), int(is_coded), int(n_cols), int(n_rows), int(should_calculate_answer)
output_dir = output_dir + "/" if output_dir[-1] != "/" else output_dir

def generate_data():

    # num workers, and number of rows per worker
    n_workers = n_procs - 1
    n_rows_per_worker = n_rows / n_workers
    if is_coded == -1:
        n_rows_per_worker = n_rows
    elif is_coded == 1:
        n_rows_per_worker = n_rows / (n_workers-1)
    elif is_coded == 2:
        n_rows_per_worker = n_rows / (n_workers-2)

    # if generating coded partitioned matrix, make sure the number of rows is a multiple
    # of n_workers-1
    if is_coded:
        if is_coded == 1:
            assert(n_rows % (n_workers-1) == 0)
        else:
            assert(n_rows % (n_workers-2) == 0)
    else:
        assert(n_rows % (n_workers)== 0)

    # Create output directory
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Total matrix, if should calculate answer
    total = []

    print("GENERATING PARTITIONED MATRIX OF SIZE %d x %d FOR %d workers" % (n_rows, n_cols, n_workers))
    print("- EACH WORKER GETS %d x %d doubles, %f MBYTES EACH" % (n_rows_per_worker, n_cols, (n_rows_per_worker * n_cols * 8) / 1000000.0))
    if not is_coded:
        random_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        for i in range(1, n_procs):
            generate_random_matrix_inplace(random_matrix, n_rows_per_worker, n_cols)
            save_matrix(random_matrix, output_dir + str(i) + ".dat")
            total += copy.deepcopy(random_matrix)
    elif is_coded == 1:
        parity_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        random_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        for i in range(1, n_procs-1):
            generate_random_matrix_inplace(random_matrix, n_rows_per_worker, n_cols)
            sum_matrices(parity_matrix, random_matrix, n_rows_per_worker, n_cols)
            save_matrix(random_matrix, output_dir + str(i) + ".dat")
            total += copy.deepcopy(random_matrix)
        save_matrix(parity_matrix, output_dir + str(n_procs-1) + ".dat")
    elif is_coded == 2:
        parity1_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        parity2_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        random_matrix = generate_empty_matrix(n_rows_per_worker, n_cols)
        for i in range(1, n_procs-2):
            generate_random_matrix_inplace(random_matrix, n_rows_per_worker, n_cols)
            save_matrix(random_matrix, output_dir + str(i) + ".dat")
            sum_matrices(parity1_matrix, random_matrix, n_rows_per_worker, n_cols)
            scal_mult_matrix_sum(parity2_matrix, random_matrix, i, n_rows_per_worker, n_cols)
            total += copy.deepcopy(random_matrix)
        save_matrix(parity1_matrix, output_dir + str(n_procs-2) + ".dat")
        save_matrix(parity2_matrix, output_dir + str(n_procs-1) + ".dat")
    else:
        print("Coded value should be 0, 1 or 2")
        sys.exit(0)

    random_vector = generate_random_vector(n_cols)
    save_vector(random_vector, output_dir + "vec.dat")

    if should_calculate_answer:
        r = matrix_vector_multiply(total, random_vector, n_rows, n_cols)
        save_vector(r, output_dir + "result.dat")
    print("DONE")

generate_data()
