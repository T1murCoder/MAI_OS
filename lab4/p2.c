#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <sys/types.h>

#define LIB_V1 "./my_lib_1.so"
#define LIB_V2 "./my_lib_2.so"

typedef float Float;
typedef int Int;

typedef Float (*DerivativeFunc)(float, float);
typedef Int (*GCFFunc)(int, int);

typedef struct {
    void* handle;
    DerivativeFunc derivative_func;
    GCFFunc gcf_func;
} DynamicLibrary;

int load_library(DynamicLibrary* lib, const char* lib_path) {
    if (lib->handle != NULL) {
        if(dlclose(lib->handle) != 0) {
            fprintf(stderr, "Ошибка закрытия библиотеки: %s\n", dlerror());
        }
        lib->handle = NULL;
        lib->derivative_func = NULL;
        lib->gcf_func = NULL;

    }

    lib->handle = dlopen(lib_path, RTLD_LAZY);
    if (lib->handle == NULL) {
        fprintf(stderr, "Ошибка загрузки библиотеки '%s': %s\n", lib_path, dlerror());
        return -1;
    }

    dlerror(); // Чистим предыдущие ошибки, как в примере из мануала: https://man7.org/linux/man-pages/man3/dlopen.3.html

    lib->derivative_func = (DerivativeFunc)dlsym(lib->handle, "Derivative");
    lib->gcf_func = (GCFFunc)dlsym(lib->handle, "GCF");

    char* error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Ошибка загрузки symbols: %s\n", error);
        dlclose(lib->handle);
        lib->handle = NULL;
        return -1;
    }

    printf("Библиотека успешно загружена: %s\n", lib_path);
    return 0;
}

void cleanup_library(DynamicLibrary* lib) {
    if (lib->handle != NULL) {
        dlclose(lib->handle);
        lib->handle = NULL;
        lib->derivative_func = NULL;
        lib->gcf_func = NULL;
    }
}

int main() {
    DynamicLibrary lib = {0};

    if (load_library(&lib, LIB_V1) != 0) {
        fprintf(stderr, "Библиотека не найдена\n");
        return 1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    bool use_first_lib = true;

    printf("Введите команды (0, 1 A deltaX, 2 A B):\n");

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (read == 0) continue;

        if (strcmp(line, "0") == 0) {
            if (use_first_lib) {
                if (load_library(&lib, LIB_V2) == 0) {
                    printf("Смена на вторую реализацию\n");
                    use_first_lib = 0;
                } else {
                    fprintf(stderr, "Ошибка смены реализации\n");
                }
            } else {
                if (load_library(&lib, LIB_V1) == 0) {
                    printf("Смена на первую реализацию\n");
                    use_first_lib = 1;
                } else {
                    fprintf(stderr, "Ошибка смены реализации\n");
                }
            }
        }
        else if (line[0] == '1') {
            if (lib.derivative_func == NULL) {
                fprintf(stderr, "Ошибка: Функция Derivative не загружена\n");
                continue;
            }

            float A, deltaX;
            if (sscanf(line, "1 %f %f", &A, &deltaX) != 2) {
                fprintf(stderr, "Неправильные аргументы для функции 1\n");
                continue;
            }
            Float result = lib.derivative_func(A, deltaX);
            printf("Производная: %f\n", result);
        } else if (line[0] == '2') {
            if (lib.gcf_func == NULL) {
                fprintf(stderr, "Ошибка: Функция GCF не загружена\n");
                continue;
            }

            int A, B;
            if (sscanf(line, "2 %d %d", &A, &B) != 2) {
                fprintf(stderr, "Неправильные аргументы для функции 2\n");
                continue;
            }
            Int result = lib.gcf_func(A, B);
            printf("GCF: %d\n", result);
        } else {
            fprintf(stderr,"Ошибка: неизвестная команда\n");
        }
    }
    if (line) free(line);
    cleanup_library(&lib);
    return 0;
}