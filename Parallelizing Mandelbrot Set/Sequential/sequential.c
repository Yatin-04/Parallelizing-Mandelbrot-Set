#include <stdio.h>
#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void compute_mandelbrot_subset(int *result, int iter_limit, int x_resolution, int y_resolution, double x_begin, double y_begin, double x_end, double y_end);

int main(int argc, char *argv[])
{
    int width = 666;
    int height = 666;
    double x_min = -2.0;
    double x_max = 1.0;
    double y_min = -1.0;
    double y_max = 1.0;
    int iter_limit = atoi(argv[1]);
    int data[width * height];

    clock_t start_time = clock();
    compute_mandelbrot_subset(data, iter_limit, width, height, x_min, y_min, x_max, y_max);
    clock_t end_time = clock();
    double computation_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Computation time: %f seconds\n", computation_time);
    printf("Iteration Number: %d\n", iter_limit);

    return 0;
}

void compute_mandelbrot_subset(int *result, int iter_limit, int x_resolution, int y_resolution, double x_begin, double y_begin, double x_end, double y_end)
{

    double x_step = (x_end - x_begin) / (x_resolution - 1);
    double y_step = (y_end - y_begin) / (y_resolution - 1);
    int i, j;

    for (i = 0; i < x_resolution * y_resolution; i++)
    {
        double complex c = x_begin + (i % x_resolution) * x_step + (y_begin + (i / x_resolution) * y_step) * I;
        double complex z = 0 + 0 * I;
        j = 0;
        while (abs(z) <= 2 && j < iter_limit)
        {
            z = z * z + c;
            j++;
        }
        result[i] = j;
    }
}
