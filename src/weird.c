#include <math.h>

// (x - 1) * (x + 3)^2 * (x - 2)^3 * (x + 1)^4
double weird1(double x) {
    double a = x - 1;
    double b = pow(x + 3, 2);
    double c = pow(x - 2, 3);
    double d = pow(x + 1, 4);
    return a * b * c * d;
}

double weird2(double x) {
    return exp(sin(x));
}
