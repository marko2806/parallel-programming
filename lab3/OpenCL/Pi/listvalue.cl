__kernel void listvalue(__global double *A, __const double value, __const int n) {
    // Get the index of the current element to be processed
    int i = get_global_id(0);
    if (i < n){
        A[i] = value;
    }
    
    
}