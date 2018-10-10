#if !defined(function_h)
#define function_h

typedef double (*Function)(double);

double area(Function f, double a, double b, double epsilon);

#endif
