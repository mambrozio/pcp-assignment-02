#if !defined(area_h)
#define area_h

typedef double (*Function)(double);

typedef struct Interval {
    double a, b;
} Interval;

double calculate_area_recursively(Function f, double a, double b, double fa,
    double fb, double epsilon);

double calculate_area_partially(Function f, Interval* i, double epsilon);

#endif
