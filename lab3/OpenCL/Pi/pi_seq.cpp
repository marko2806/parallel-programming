#include <stdio.h>
#include <stdlib.h>
#include "cfdio.h"

int main_pi_seq() {
    int n = 1024;
    int num_of_iterations = 1000000;
    int r = 10000;
    double tstart = gettime();

    

    int* C = new int[n];
    for (int i = 0; i < n; i++) {
        C[i] = 0;
        unsigned long seed = i;
        for (int j = 0; j < num_of_iterations; j++) {
            seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
            unsigned int x = seed >> 16;
            x = x % (2 * r + 1);

            seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
            unsigned int y = seed >> 16;
            y = y % (2 * r + 1);

            if ((x - r) * (x - r) + (y - r) * (y - r) < r * r) {
                C[i] += 1;
            }
        }
    }
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += C[i];

    double tstop = gettime();
    double ttot = tstop - tstart;

    printf("PI: %f\n", 4.0 * (double)sum / (num_of_iterations * n));
    printf("Time: %f\n", ttot);
    delete C;
    return 0;
}