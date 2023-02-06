#include "copylist.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <CL/cl.h>
#include "jacobian.h"
#define MAX_SOURCE_SIZE (0x100000)

static const int LIST_SIZE = 2050 * 2050;

static cl_command_queue command_queue;
static cl_mem a_mem_obj;
static cl_mem b_mem_obj;
static cl_kernel kernel;
static cl_program program;
static cl_context context;

void jacobian_init() {
    int i;


    // Load the kernel source code into the array source_str
    FILE* fp;
    char* source_str;
    size_t source_size;

    fopen_s(&fp, "jacobi.cl", "r");
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
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    // Create a command queue
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
    // Create memory buffers on the device for each vector 
    a_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(double), NULL, &ret);
    b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(double), NULL, &ret);
    // Create a program from the kernel source
    program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

    // Build the program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Create the OpenCL kernel
    kernel = clCreateKernel(program, "jacobi", &ret);


    int m = 2048;
    int n = 2048;
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void*)&m);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void*)&n);
    //printf("ret at %d is %d\n", __LINE__, ret);

}

void calc_jacobian(double* A, double* B, int m, int n) {
    size_t local_item_size = 64; // Divide work items into groups of 64
    size_t global_item_size = ceil(((m + 2) * (n + 2)) / (float)local_item_size) * local_item_size; // Process the entire lists



    int remaining = n;
    int ret;
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(double), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(double), B, 0, NULL, NULL);
    //printf("ret at %d is %d\n", __LINE__, ret);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    //printf("ret at %d is %d\n", __LINE__, ret);
    // Read the memory buffer C on the device to the local variable C
    ret = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(double), A, 0, NULL, NULL);

    /*for (int offset = 0; offset <= n; offset += LIST_SIZE) {

        remaining -= LIST_SIZE;
    }*/

}

void jacobian_cleanup() {
    // Clean up
    int ret;
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
}