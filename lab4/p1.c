#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/my_lib.h"

int main() {
    char* line = NULL;
    size_t len = 0;
    size_t read;

    printf("Введите команды (1 A deltaX, 2 A B):\n");

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        if (strlen(line) == 0) continue;

        if (line[0] == '1') {
            float A, deltaX;
            if (sscanf(line, "1 %f %f", &A, &deltaX) != 2) {
                fprintf(stderr, "Неправильные аргументы для функции 1\n");
                continue;
            }
            Float result = Derivative(A, deltaX);
            printf("Производная: %f\n", result);
        } else if (line[0] == '2') {
            int A, B;
            if (sscanf(line, "2 %d %d", &A, &B) != 2) {
                fprintf(stderr, "Неправильные аргументы для функции 2\n");
                continue;
            }
            Int result = GCF(A, B);
            printf("GCF: %d\n", result);
        } else {
            fprintf(stderr,"Ошибка: неизвестная команда\n");
        }
    }
    free(line);
    return 0;
}