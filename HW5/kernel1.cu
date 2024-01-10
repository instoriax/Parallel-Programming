#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__global__ void mandelKernel(float x0, float y0, float dx, float dy, int count , int resX, int resY, int* output) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i>=resX)
        return;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if(j>=resY)
        return;
    float c_re = x0 + i * dx;
    float c_im = y0 + j * dy;
    float z_re = c_re, z_im = c_im;
    int it;
    for (it = 0; it < count; ++it)
    {
        if (z_re * z_re + z_im * z_im > 4.f)
        break;
        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }
    output[j*resX + i]=it;
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int *output;
    cudaMalloc(&output, resX*resY*sizeof(int));
    int *host=(int*)malloc(resX*resY*sizeof(int));
    dim3 threadsPerBlock(32, 32);
    int x=resX%32? (resX/32)+1 : (resX/32);
    int y=resY%32? (resY/32)+1 : (resY/32);
    dim3 numBlocks(x,y);
    mandelKernel<<<numBlocks, threadsPerBlock>>>(lowerX, lowerY, stepX, stepY, maxIterations, resX, resY, output);
    cudaMemcpy(host, output, resX * resY * sizeof(int), cudaMemcpyDeviceToHost);
    memcpy(img, host, resX*resY*sizeof(int));
    cudaFree(output);
    free(host);
}
