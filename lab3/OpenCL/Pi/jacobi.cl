__kernel void jacobi(__global double *A, __global double *B, __const int m, __const int n) {
    int i = get_global_id(0);
    
    if (i >= (m + 2) * (n + 2) || i % (m + 2) == 0 || i < (m + 2) || i % (m + 2) == (n + 1) || i >= (n + 1) * (m + 2) ){
        return;
    }

    A[i]=0.25*(B[i-(m+2)]+B[i+(m+2)]+B[i-1]+B[i+1]);
}