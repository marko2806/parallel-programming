#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <math.h>
#include "cfdio.h"

#define MAX_SOURCE_SIZE (0x100000)


int main_prim() {
    printf("Started\n");
    double tstart = gettime();

    // Create the two input vectors
    int i;
    const int numbers = 1024 * 1024;
    size_t global_item_size = 1024 * 128;
    size_t local_item_size = 128;
    int num_of_tasks = ceil(numbers / global_item_size);


    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, "prim.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;


    cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    cl_platform_id* platforms = NULL;
    platforms = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));

    ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
    ret = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, global_item_size * sizeof(int), NULL, &ret);

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "prim", &ret);
        

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&c_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&global_item_size);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_int), (void*)&num_of_tasks);
    

    int* C = (int*)malloc(sizeof(int) * global_item_size);
    //ret = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, num_of_tasks * sizeof(int), C, 0, NULL, NULL);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, global_item_size * sizeof(int), C, 0, NULL, NULL);
    
    int sum = 0;
    for (i = 0; i < global_item_size; i++) {
        sum = sum + C[i];
    }

    double tstop = gettime();
    double ttot = tstop - tstart;
    printf("Number of prime numbers: %d\n", sum);
    printf("Time: %f\n", ttot);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source_str);
    free(C);
    free(platforms);
}
