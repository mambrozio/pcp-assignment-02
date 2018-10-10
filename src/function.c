#include "function.h"

#include <stdlib.h>

static double area_trapezoid(double b1, double b2, double h);

double area(Function f, double a, double b, double epsilon) {
    double fa = f(a);
    double fb = f(b);
    double trapezoid = area_trapezoid(fa, fb, b - a);

    double c = (a + b) / 2.0;
    double fc = f(c);

    double left_trapezoid = area_trapezoid(fa, fc, c - a);
    double right_trapezoid = area_trapezoid(fc, fb, b - c);

    double divided_trapezoid = left_trapezoid + right_trapezoid;
    double diff = abs(trapezoid - divided_trapezoid);

    if (diff > epsilon) {
        return area(f, a, c, epsilon) + area(f, c, b, epsilon);
    } else {
        return divided_trapezoid;
    }
}

// ==================================================
//
//  Auxiliary
//
// ==================================================

static double area_trapezoid(double b1, double b2, double h) {
    return (b1 + b2) * h / 2;
}
