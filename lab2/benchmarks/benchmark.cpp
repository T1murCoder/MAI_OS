#include <iostream>
#include <chrono>
#include <fstream>
#include <random>
#include "../include/gaussian.h"

constexpr unsigned int SEED = 52;

void generate_random_system(double** a, double* y, int n, unsigned int seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(1.0, 10.0); // ТОЛЬКО положительные числа
    
    // Генерируем случайное истинное решение
    double* x_true = new double[n];
    for (int i = 0; i < n; i++) {
        x_true[i] = dis(gen);
    }
    
    // Генерируем матрицу
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = dis(gen);
        }
        // Усиливаем диагональ в 10 раз
        a[i][i] = dis(gen) * n * 10.0;
    }
    
    // Вычисляем правую часть: y = A * x_true
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j < n; j++) {
            y[i] += a[i][j] * x_true[j];
        }
    }
    
    delete[] x_true;
}

void copy_system(double** dest_a, double* dest_y, double** src_a, double* src_y, int n) {
    for (int i = 0; i < n; i++) {
        dest_y[i] = src_y[i];
        for (int j = 0; j < n; j++) {
            dest_a[i][j] = src_a[i][j];
        }
    }
}

int main() {
    const int n = 10000; // Размер матрицы
    const int num_runs = 2; // Количество прогонов для усреднения
    int thread_counts[] = {1, 2, 3, 4, 6, 8, 12, 16};
    
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║   БЕНЧМАРК МЕТОДА ГАУССА С THREAD POOL                 ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "📊 Размер матрицы: " << n << "×" << n << std::endl;
    std::cout << "🔄 Количество прогонов: " << num_runs << std::endl;
    std::cout << "⚙️  Тестируемые конфигурации: ";
    for (int t : thread_counts) {
        std::cout << t << " ";
    }
    std::cout << "\n\n";
    
    // Создаем исходную систему
    double **original_a = new double*[n];
    double *original_y = new double[n];
    for (int i = 0; i < n; i++) {
        original_a[i] = new double[n];
    }
    
    std::cout << "Генерация тестовой системы...\n";
    generate_random_system(original_a, original_y, n, SEED);
    std::cout << "Система сгенерирована!\n\n";
    
    // Файл для результатов
    std::ofstream results("benchmark_results.txt");
    results << "# threads time_seconds\n";
    
    std::cout << "═══════════════════════════════════════════════════════════════════\n";
    std::cout << " Потоков │ Время (сек) │ Ускорение │ Эффективность │ Статус\n";
    std::cout << "═══════════════════════════════════════════════════════════════════\n";
    
    double time_single_thread = 0;
    
    for (int num_threads : thread_counts) {
        double total_time = 0;
        
        std::cout << "    " << num_threads << "    │ ";
        std::cout.flush();
        
        for (int run = 0; run < num_runs; run++) {
            // Копия системы
            double **a = new double*[n];
            double *y = new double[n];
            for (int i = 0; i < n; i++) {
                a[i] = new double[n];
            }
            copy_system(a, y, original_a, original_y, n);
            
            // Измеряем время
            auto start = std::chrono::high_resolution_clock::now();
            
            // Передаем количество потоков в функцию
            double* x = gauss(a, y, n, num_threads);
            
            auto end = std::chrono::high_resolution_clock::now();
            
            std::chrono::duration<double> elapsed = end - start;
            total_time += elapsed.count();
            
            // Очистка
            if (x) delete[] x;
            for (int i = 0; i < n; i++) {
                delete[] a[i];
            }
            delete[] a;
            delete[] y;
        }
        
        double avg_time = total_time / num_runs;
        
        if (num_threads == 1) {
            time_single_thread = avg_time;
        }
        
        double speedup = time_single_thread / avg_time;
        double efficiency = (speedup / num_threads) * 100.0;
        
        printf("%10.4f │  %7.2fx │    %6.1f%%    │ ", avg_time, speedup, efficiency);
        
        if (efficiency > 80) {
            std::cout << "🟢 Отлично\n";
        } else if (efficiency > 60) {
            std::cout << "🟡 Хорошо\n";
        } else if (efficiency > 40) {
            std::cout << "🟠 Средне\n";
        } else {
            std::cout << "🔴 Плохо\n";
        }
        
        results << num_threads << " " << avg_time << "\n";
    }
    
    std::cout << "═══════════════════════════════════════════════════════════════════\n\n";
    
    results.close();
    
    // Очистка
    for (int i = 0; i < n; i++) {
        delete[] original_a[i];
    }
    delete[] original_a;
    delete[] original_y;
    
    std::cout << "Результаты сохранены в 'benchmark_results.txt'\n";
    
    return 0;
}