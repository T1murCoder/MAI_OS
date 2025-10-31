#include "../include/gaussian.h"
#include "../include/worker_pool.h"
#include <iostream>
#include <algorithm>
#include <cmath>

static const double EPS = 0.00001;

void sysout(double** a, double* y, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      std::cout << a[i][j] << "*x" << j;
      if (j < n - 1)
        std::cout << " + ";
    }
    std::cout << " = " << y[i] << std::endl;
  }
  return;
}

FindMaxCoefResult find_max_coef_row_idx(double **a, int n, int k) {
    int max = std::abs(a[k][k]);
    int index = k;
    for (int i = k + 1; i < n; i++) {
        if (std::abs(a[i][k]) > max) {
            max = std::abs(a[i][k]);
            index = i;
        }
    }
    if (max < EPS) {
        return {-1, -1}; // нулевой столбец
    }
    return {0, index};
}

void normalize_row_task(void* arg) {
    NormalizeRowArgs* args = (NormalizeRowArgs*) arg;
    double divisor = args->row[args->k];
    
    if (std::abs(divisor) < EPS) return;

    for (int i = args->k; i < args->n; i++) {
        args->row[i] /= divisor;
    }

    *(args->y_ptr) /= divisor;
}

void subtract_from_task(void* arg) {
    SubtractFromArgs* args = (SubtractFromArgs*) arg;
    if (std::abs(args -> row[args -> k]) < EPS) return;

    for (int i = args -> k; i < args -> n; i++) {
        args -> from[i] -= args -> row[i];
    }

    *(args -> y_from) -= args->y_subtract;
}

double* gauss(double** a, double* y, int n, size_t num_threads) {
    double *x = nullptr;
    int k = 0;
    try {
    x = new double[n];
    WorkerPool pool(num_threads - 1);

    while (k < n) {
        FindMaxCoefResult res = find_max_coef_row_idx(a, n, k);
        if (res.error != 0) {
            std::cout << "Решение невозможно получить из-за нулевого столбца" << std::endl;
            
            delete x;

            return nullptr;
        }

        std::swap(a[k], a[res.max_coef_idx]);
        std::swap(y[k], y[res.max_coef_idx]);

        // Нормализация уравнений

        std::vector<NormalizeRowArgs> normalize_args_array;
        for (size_t i = k; i < n; i++) {
            normalize_args_array.push_back({a[i], &y[i], n, k});
        }

        for (size_t i = 0; i < normalize_args_array.size(); i++) {
            pool.enqueue(normalize_row_task, &normalize_args_array[i]);
        }
        pool.wait();

        // Вычитание
        std::vector<SubtractFromArgs> subtract_args_array;
        for (size_t i = k +1; i < n; i++) {
            subtract_args_array.push_back({a[i], a[k], &y[i], y[k], n, k});
        }

        for (size_t i = 0; i < subtract_args_array.size(); i++) {
            pool.enqueue(subtract_from_task, &subtract_args_array[i]);
        }
        pool.wait();

        k++;
    }
    for (k = n - 1; k >= 0; k--) {
        x[k] = y[k];
        for (int i = 0; i < k; i++) {
            y[i] = y[i] - a[i][k] * x[k];
        }
    }
    return x;
    } catch (const std::exception& e) {
        // Очистка и проброс исключения дальше
        delete[] x;
        throw;
    }
}