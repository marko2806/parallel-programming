#include <stdio.h>
#include <stdlib.h>

#include "cfdio.h"

#include <CL/cl.h>
#include <math.h>

#define MAX_SOURCE_SIZE (0x100000)


int main_pi() {
    printf("Started running\n");
    
    double tstart = gettime();

    const int n = 1024;
    int r = 10000;
    int num_of_iterations = 1000000;

    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, "kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    //printf("Loaded kernel\n");
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // Get platform and device information
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;


    cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    cl_platform_id* platforms = NULL;
    platforms = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));

    ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
    ret = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 1,
        &device_id, &ret_num_devices);
    


    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, n * sizeof(int), NULL, &ret);
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "pi_calc", &ret);

    size_t max_available_local_wg_size;
    ret = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &max_available_local_wg_size, NULL);
    size_t maxWorkGroupSize;
    clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
    
    //printf("%d\n", max_available_local_wg_size);
    //printf("%d\n", maxWorkGroupSize);

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&c_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_int), (void*)&r);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_int), (void*)&num_of_iterations);
    ret = clSetKernelArg(kernel, 3, sizeof(int), &n);


    // Execute the OpenCL kernel on the list
    size_t local_item_size = 64;
    size_t global_item_size = ceil(n / local_item_size) * local_item_size; // Process the entire lists
    //printf("Local item size: %d\n", local_item_size);
    //printf("Global item size: %d\n", global_item_size);
    


    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    int* C = (int*)malloc(sizeof(int) * n);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,n * sizeof(int), C, 0, NULL, NULL);

    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += C[i];

    double tstop = gettime();
    double ttot = tstop - tstart;
    
    printf("PI: %f\n", 4.0 * (double)sum / (num_of_iterations * n));
    printf("Time: %f\n", ttot);
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(C);
    free(source_str);
    free(platforms);
}