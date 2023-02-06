__kernel void prim(__global int *C, __const int n, __const int num_of_numbers) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    
    
    if(i >= n){
        return;
    }
    
    C[i] = 0;
    int start = i * num_of_numbers + 1;      //pocinje se od 1
    int end = (i + 1) * num_of_numbers + 1;  //pocinje se od 1

    
    for(int n = start; n < end; n++){
        int prim = 1;
        for(int j = 2; j < n; j++){
            if(n % j == 0){
                prim = 0;
                break;
            }
        }
        if(prim == 1){
            C[i] = C[i] + 1;
        }
    }

    
}