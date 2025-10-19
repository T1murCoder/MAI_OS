#include <iostream>
#include "include/gaussian.h"

constexpr size_t NUM_WORKER_THREADS = 3; // Количество потоков для воркеров
static_assert(NUM_WORKER_THREADS >= 1, "Количество потоков для воркеров должно быть минимум 1");

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
    x = gauss(a, y, n, NUM_WORKER_THREADS);
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