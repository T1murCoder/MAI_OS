#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <algorithm>
#include <queue>
#include <functional>
#include "include/thread_pool.h"
#include <cmath>

constexpr size_t NUM_WORKER_THREADS = 3; // Количество потоков для воркеров
static_assert(NUM_WORKER_THREADS >= 1, "Количество потоков для воркеров должно быть минимум 1");

static const double EPS = 0.00001;

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

double* gauss(double** a, double* y, int n) {
    double *x;
    int k = 0;
    x = new double[n];
    ThreadPool pool(NUM_WORKER_THREADS);

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
}

int main() {
    double **a, *y, *x;
    int n;

    std::cout << ("Введите количество уравнений: ");

    std::cin >> n;

    a = new double*[n];
    y = new double[n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[n];
        for (int j = 0; j < n; j++) {
            std::cout << "a[" << i << "][" << j << "]= ";
            std::cin >> a[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        std::cout << "y[" << i << "]= ";
        std::cin >> y[i];
    }

    sysout(a, y, n);
    x = gauss(a, y, n);
    for (int i = 0; i < n; i++) {
        std::cout << "x[" << i << "]=" << x[i] << std::endl;
    }

    for (int i = 0; i < n; i++) { delete[] a[i];}
    delete[] a;
    delete[] y;
    delete[] x;
    std::cin.get();
    return 0;
}