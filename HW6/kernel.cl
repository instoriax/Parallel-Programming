__kernel void convolution(int filterWidth, __global float *filter, int imageHeight, int imageWidth,
            __global float *inputImage, __global float *outputImage) 
{
    const int j = get_global_id(0);
    const int i = get_global_id(1);
    int halffilterSize = filterWidth / 2;
    float sum = 0;
    for (int k = -halffilterSize; k <= halffilterSize; k++)
    {
        for (int l = -halffilterSize; l <= halffilterSize; l++)
        {
            if (i + k >= 0 && i + k < imageHeight &&
                j + l >= 0 && j + l < imageWidth)
            {
                sum += inputImage[(i + k) * imageWidth + j + l] *
                       filter[(k + halffilterSize) * filterWidth +
                              l + halffilterSize];
            }
        }
    }
    outputImage[i * imageWidth + j] = sum;
}
