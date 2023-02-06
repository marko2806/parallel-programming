#include <stdio.h>
#include <stdlib.h>

#include "cfdio.h"

int main_prim_seq() {
    int n = 1024 * 1024;

    double tstart = gettime();

    int sum = 0;
    for (int i = 0; i < n; i++) {
        //printf("%d\n", i);
        int prim = 1;
        for (int j = 2; j < i + 1; j++) {
            if ((i + 1) % j == 0) {
                prim = 0;
                break;
            }
        }
        if (prim) {
            sum += 1;
        }
    }

    
    double tstop = gettime();
    double ttot = tstop - tstart;
    printf("Time: %f\n", ttot);
    printf("Number of prime numbers: %d\n", sum);
    return 0;
}