__kernel void pi_calc(__global int *C, __const int r, __const int num_of_iterations, __const int n) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    if(i >= n){
        return;
    }
    
    // Java random...
    ulong seed = i;
    
    C[i] = 0;
    for(int j = 0; j < num_of_iterations; j++){
        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        uint x = seed >> 16;
        x = x % (2 * r + 1);

        seed = (seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
        uint y = seed >> 16;
        y = y % (2 * r + 1);

        if((x - r) * (x - r) + (y - r) * (y - r)  < r * r){
            C[i] = C[i] + 1;
        }
    }
}