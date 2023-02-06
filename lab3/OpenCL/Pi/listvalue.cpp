#include "listvalue.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#define MAX_SOURCE_SIZE (0x100000)


void list_value(double* A, double value, int n) {
    //printf("started running\n");

    // Create the two input vectors
    int i;
    const int LIST_SIZE = n;
    


    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, "listvalue.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
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
    ret = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    // Create memory buffers on the device for each vector 
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, LIST_SIZE * sizeof(double), NULL, &ret);
    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "listvalue", &ret);
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)(&a_mem_obj));
    ret = clSetKernelArg(kernel, 1, sizeof(double), (void*)&value);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void*)&n);
    // Execute the OpenCL kernel on the list
    size_t local_item_size = 64; // Divide work items into groups of 64
    size_t global_item_size = ceil(n / (float)local_item_size) * local_item_size;; // Process the entire lists

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(double), A, 0, NULL, NULL);

    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
}
