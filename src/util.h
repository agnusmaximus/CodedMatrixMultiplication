#ifndef UTIL
#define UTIL

#include <string>
#include <time.h>
#include <iostream>
#include <cblas.h>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string.h>
#include <stdio.h>

using namespace std;

string to_string_util(int val) {
  return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << val)).str();
}

long long int get_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void matrix_vector_multiply(double *mat, double *vec, double *out, int m_cols, int n_rows) {
    cblas_dgemv(CblasRowMajor,  CblasNoTrans,  n_rows, m_cols, 1, mat, m_cols, vec, 1, 1, out, 1);
}

double *load_vector(int vector_length, string fname) {
    double *vec = (double *)malloc(sizeof(double) * vector_length);
    ifstream in(fname.c_str());
    if (!in) {
	cout << "Error: " << fname << " could not be opened!" << endl;
	exit(0);
    }
    for (int i = 0; i < vector_length; i++) {
	in >> vec[i];
    }
    in.close();
    return vec;
}

double *load_matrix(int n_rows, int n_cols, string fname) {
    double *matrix = (double *)malloc(sizeof(double) * n_rows * n_cols);
    ifstream in(fname.c_str());
    if (!in) {
	cout << "Error: " << fname << " could not be opened!" << endl;
	exit(0);
    }
    for (int i = 0; i < n_rows; i++) {
	for (int j = 0; j < n_cols; j++) {
	    in >> matrix[i*n_cols+j];
	}
    }
    in.close();
    return matrix;
}

void check_correct(double *out, int n, string result_vector_fname) {
    ifstream in(result_vector_fname.c_str());
    if (!in) {
	cout << "Error: " << result_vector_fname << " could not be opened!" << endl;
	exit(0);
    }
    vector<double> correct_result;
    double cur_element = 0;
    while (in >> cur_element) {
	correct_result.push_back(cur_element);
    }
    if (correct_result.size() != n) {
	cout << "INCORRECT: WRONG SIZE" << endl;
	exit(0);
    }
    for (int i = 0; i < n; i++) {
	if (correct_result[i] != out[i]) {
	    cout << "INCORRECT: WRONG ELEMENT " << correct_result[i] << " VS " << out[i] << " AT INDEX " << i << endl;
	    exit(0);
	}
    }
    cout << "Checks pass!" << endl;
}

void get_incomplete_worker(int *completed, int n_workers, int *out) {
    for (int i = 0; i < n_workers; i++) {
	if (!completed[i]) {
	    *out = i;
	    return;
	}
    }
}

void get_both_incomplete_workers(int *completed, int n_workers, int *smaller, int *larger) {
    int i;
    for (i = 0; i < n_workers; i++) {
	if (!completed[i]) {
	    *smaller = i++;
	    break;
	}
    }
    for (; i < n_workers; i++) {
	if (!completed[i]) {
	    *larger = i;
	    return;
	}
    }
}

void vector_vector_sum(double *v1, double *v2, double *out, int n_rows) {
    for (int i = 0; i < n_rows; i++) {
	out[i] = v1[i] + v2[i];
    }
}

void vector_vector_subtract(double *v1, double *v2, double *out, int n_rows) {
    for (int i = 0; i < n_rows; i++) {
	out[i] = v1[i] - v2[i];
    }
}

void vector_vector_scalar_multiply(double *v1, double *out, double k, int n_rows) {
    for (int i = 0; i < n_rows; i++) {
	out[i] = v1[i] * k;
    }
}

void vector_vector_add_scalark(double *v1, double *v2, double *out, double k, int n_rows) {
    for (int i = 0; i < n_rows; i++) {
	out[i] = v1[i] + v2[i]*k;
    }
}

#endif
