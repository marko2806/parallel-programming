#pragma once

void jacobian_init();
void calc_jacobian(double* A, double* B, int m, int n);
void jacobian_cleanup();