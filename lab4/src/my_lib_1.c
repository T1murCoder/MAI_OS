#include "../include/my_lib.h"
#include <math.h>
#include <float.h>

Float Derivative(float A, float deltaX) {
    float dx = fabsf(deltaX);

    if (dx == 0.0f || dx < FLT_EPSILON) {
        return (Float)NAN;
    }

    float f_plus = cosf(A + dx);
    float f_at_A = cosf(A);

    return (Float)((f_plus - f_at_A) / dx);
}

Int GCF(int A, int B) {
    while (B != 0) {
        int temp = B;
        B = A % B;
        A = temp;
    }
    return A;
}