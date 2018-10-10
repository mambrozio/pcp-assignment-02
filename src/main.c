#include <math.h>
#include <stdio.h>

#include "function.h"

#define EPSILON 0.0000000000000001

int main(void) {
    double x = area(exp, 0, 10, EPSILON);
    printf("%.50f\n", x);
    return 0;
}