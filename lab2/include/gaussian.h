#pragma once

#include <vector>

struct NormalizeRowArgs {
    double* row;
    double* y_ptr;
    int n;
    int k;
};

struct SubtractFromArgs {
    double* from;
    double* row;
    double* y_from;
    double y_subtract;
    int n;
    int k;
};

struct FindMaxCoefResult {
    int error;
    int max_coef_idx;
};

void sysout(double** a, double* y, int n);
FindMaxCoefResult find_max_coef_row_idx(double **a, int n, int k);
void normalize_row_task(void* arg);
void subtract_from_task(void* arg);
double* gauss(double** a, double* y, int n, std::size_t num_threads = 3);