#include <cuda.h>
#include <stdio.h>
#include "cuda_f.h"

__global__ void PrintId()
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  printf("Cuda thread id = %d\n", idx);
}

extern "C" void test()
{
    dim3 blocks(4,1,1);
    dim3 thread(1024,1,1);

    cudaEvent_t start, stop;
    float gpuTime;
    cudaEventCreate( &start );
    cudaEventCreate( &stop );
    cudaEventRecord( start, 0 );

    PrintId<<<blocks, thread>>>();
    cudaDeviceSynchronize();

    cudaEventRecord( stop, 0 );
    cudaEventSynchronize(stop);
    cudaEventElapsedTime( &gpuTime, start, stop );
    printf("time spent executing by the GPU: %.2f millseconds\n", gpuTime);
    cudaEventDestroy( start);
    cudaEventDestroy( stop);
}