#include "area.h"

#include <math.h>
#include <stdlib.h>

static double calculate_trapezoid_area(double b1, double b2, double h);

double calculate_area_recursively(Function f, double a, double b, double fa,
    double fb, double epsilon) {

    double c = (a + b) / 2.0;
    double fc = f(c);

    double left_trapezoid = calculate_trapezoid_area(fa, fc, c - a);
    double right_trapezoid = calculate_trapezoid_area(fc, fb, b - c);
    double undivided_trapezoid = calculate_trapezoid_area(fa, fb, b - a);
    double divided_trapezoid = left_trapezoid + right_trapezoid;

    double diff = fabs(undivided_trapezoid - divided_trapezoid);

    if (diff > epsilon) {
        double left = calculate_area_recursively(f, a, c, fa, fc, epsilon);
        double right = calculate_area_recursively(f, c, b, fc, fb, epsilon);
        return left + right;
    } else {
        return divided_trapezoid;
    }
}

double calculate_area_partially(Function f, Interval* i, double epsilon) {
    double c = (i->a + i->b) / 2.0;

    double fa = f(i->a);
    double fb = f(i->b);
    double fc = f(c);

    double left_trapezoid = calculate_trapezoid_area(fa, fc, c - i->a);
    double right_trapezoid = calculate_trapezoid_area(fc, fb, i->b - c);
    double undivided_trapezoid = calculate_trapezoid_area(fa, fb, i->b - i->a);
    double divided_trapezoid = left_trapezoid + right_trapezoid;

    double diff = fabs(undivided_trapezoid - divided_trapezoid);

    return (diff > epsilon) ? INFINITY : divided_trapezoid;
}

// ==================================================
//
//  Auxiliary
//
// ==================================================

static double calculate_trapezoid_area(double b1, double b2, double h) {
    return fabs((b1 + b2) * h / 2);
}
