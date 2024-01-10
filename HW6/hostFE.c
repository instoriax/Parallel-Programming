#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"


void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth;
    int size=imageHeight*imageWidth;
    cl_command_queue myqueue = clCreateCommandQueue(*context, *device, 0, &status);
    cl_kernel mykernel = clCreateKernel (*program , "convolution" ,&status);
    cl_mem d_ip = clCreateBuffer(*context, CL_MEM_READ_ONLY,size*sizeof(float),NULL, &status);
    status = clEnqueueWriteBuffer(myqueue, d_ip, CL_TRUE, 0, size*sizeof(float), (void *)inputImage, 0, NULL, NULL);
    cl_mem f_ip = clCreateBuffer(*context, CL_MEM_READ_ONLY,filterSize*sizeof(float),NULL, &status);
    status = clEnqueueWriteBuffer(myqueue, f_ip, CL_TRUE, 0, filterSize*sizeof(float), (void *)filter, 0, NULL, NULL);
    cl_mem d_op = clCreateBuffer(*context, CL_MEM_WRITE_ONLY,size*sizeof(float),NULL, &status);
    status=clSetKernelArg(mykernel, 0, sizeof(int),(void *)&filterWidth);
    status=clSetKernelArg(mykernel, 1, sizeof(cl_mem),&f_ip);
    status=clSetKernelArg(mykernel, 2, sizeof(int),(void *)&imageHeight);
    status=clSetKernelArg(mykernel, 3, sizeof(int),(void *)&imageWidth);
    status=clSetKernelArg(mykernel, 4, sizeof(cl_mem),&d_ip);
    status=clSetKernelArg(mykernel, 5, sizeof(cl_mem),&d_op);
    size_t localws[2] = {50,20};
    size_t globalws[2] = {imageWidth, imageHeight};
    status=clEnqueueNDRangeKernel(myqueue , mykernel,2, 0, globalws, localws,0, NULL, NULL);
    status=clEnqueueReadBuffer(myqueue, d_op,CL_TRUE, 0, size*sizeof(float), (void *) outputImage,NULL, NULL, NULL);
    clReleaseMemObject(d_ip);
    clReleaseMemObject(f_ip);
    clReleaseMemObject(d_op);
}