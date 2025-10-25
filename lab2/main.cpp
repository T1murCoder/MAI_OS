#include <iostream>
#include <cstring>
#include "include/gaussian.h"

constexpr size_t DEFAULT_NUM_THREADS = 3; // Количество потоков по умолчанию
constexpr size_t MIN_THREADS = 2; // Минимум потоков


void print_usage(const char* program_name) {
    std::cout << "Использование: " << program_name << " [-t <количество_потоков>]" << std::endl;
    std::cout << "  -t <количество_потоков>  Количество потоков (минимум " 
              << MIN_THREADS << ", по умолчанию " 
              << DEFAULT_NUM_THREADS << ")" << std::endl;
    std::cout << "  -h, --help               Показать эту справку" << std::endl;
}

int main(int argc, char* argv[]) {

    size_t num_threads = DEFAULT_NUM_THREADS;
    
    // Обработка аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                int threads = std::atoi(argv[i + 1]);
                if (threads < static_cast<int>(MIN_THREADS)) {
                    std::cerr << "Ошибка: количество потоков должно быть минимум " 
                              << MIN_THREADS << std::endl;
                    return 1;
                }
                num_threads = threads;
                i++; // Пропускаем следующий аргумент
            } else {
                std::cerr << "Ошибка: не указано количество потоков после -t" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Ошибка: неизвестный параметр " << argv[i] << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
    std::cout << "Используется потоков: " << num_threads << std::endl;

    double **a, *y, *x;
    int n;

    // std::cout << ("Введите количество уравнений: ");

    std::cin >> n;

    a = new double*[n];
    y = new double[n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[n];
        for (int j = 0; j < n; j++) {
            // std::cout << "a[" << i << "][" << j << "]= ";
            std::cin >> a[i][j];
        }
    }

    for (int i = 0; i < n; i++) {
        // std::cout << "y[" << i << "]= ";
        std::cin >> y[i];
    }

    // sysout(a, y, n);
    x = gauss(a, y, n, num_threads);
    for (int i = 0; i < n; i++) {
        std::cout << "x[" << i << "]=" << x[i] << std::endl;
    }

    for (int i = 0; i < n; i++) { delete[] a[i];}
    delete[] a;
    delete[] y;
    delete[] x;

    return 0;
}