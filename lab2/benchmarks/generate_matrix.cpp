#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>

constexpr unsigned int SEED = 52;

void generate_random_system(double** a, double* y, int n, unsigned int seed, const std::string& filename) {
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
    
    // Сохраняем в файл
    std::ofstream file(filename);
    if (file.is_open()) {
        // Записываем размер системы
        file << n << std::endl;
        
        // Записываем матрицу A
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                file << a[i][j];
                if (j < n - 1) file << " ";
            }
            file << std::endl;
        }
        
        // Записываем вектор y
        for (int i = 0; i < n; i++) {
            file << y[i];
            if (i < n - 1) file << " ";
        }
        file << std::endl;
        
        file.close();
        std::cout << "Матрица сохранена в файл: " << filename << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи" << std::endl;
    }
    
    delete[] x_true;
}

int main() {
    int n;
    std::string filename;
    
    std::cout << "Введите размер матрицы: ";
    std::cin >> n;
    
    std::cout << "Введите имя файла для сохранения: ";
    std::cin >> filename;
    
    // Выделяем память
    double** a = new double*[n];
    double* y = new double[n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[n];
    }
    
    // Генерируем и сохраняем
    generate_random_system(a, y, n, SEED, filename);
    
    // Освобождаем память
    for (int i = 0; i < n; i++) {
        delete[] a[i];
    }
    delete[] a;
    delete[] y;
    
    return 0;
}