from __future__ import print_function
import sys
import random
import os
import numpy as np

def save_matrix(m, output):
    f = open(output, "w")
    for i in range(len(m)):
        print(" ".join([str(x) for x in m[i]]), file=f)
    f.close()

def save_vector(m, output):
    f = open(output, "w")
    for i in range(len(m)):
        print(str(m[i]) + " ", file=f)
    f.close()

def sum_matrices(m1, m2, n_rows, n_cols):
    # m1 = m1 + m2
    for i in range(n_rows):
        for j in range(n_cols):
            m1[i][j] += m2[i][j]

def sub_matrices(m1, m2, n_rows, n_cols):
    # m1 = m1 - m2
    for i in range(n_rows):
        for j in range(n_cols):
            m1[i][j] -= m2[i][j]

def scal_mult_matrix_sum(out, m, s, n_rows, n_cols):
    for i in range(n_rows):
        for j in range(n_cols):
            out[i][j] += m[i][j] * s

def scal_mult_matrix_ret(m, s, n_rows, n_cols):
    ret = [[0 for i in range(n_cols)] for j in range(n_rows)]
    for i in range(n_rows):
        for j in range(n_cols):
            ret[i][j] = m[i][j] * s
    return ret

def generate_random_matrix(n_rows, n_cols):
    return [[np.random.uniform(low=0,high=100) for i in range(n_cols)] for x in range(n_rows)]

def generate_random_matrix_inplace(mat, n_rows, n_cols):
    for i in range(n_rows):
        for j in range(n_cols):
            mat[i][j] = random.randint(0, 100)

def generate_empty_matrix(n_rows, n_cols):
    return [[0 for i in range(n_cols)] for x in range(n_rows)]

def generate_random_vector(n):
    return [random.randint(0, 100) for x in range(n)]

def matrix_vector_multiply(matrix, vector, n_rows, n_cols):
    result = [0 for x in range(n_rows)]
    for i in range(n_rows):
        for j in range(n_cols):
            result[i] += matrix[i][j] * vector[j]
    return result
