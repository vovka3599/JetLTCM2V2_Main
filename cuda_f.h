extern "C" void test();

/*
Main functions CUDA:
cudaMallocManaged           - shared memory allocation
cudaMalloc                  - memory allocation
cudaMemcpyToSymbol          - data copy to __constant__ memory
cudaMemcpy                  - data copy (host to device OR device to host)
cufftExecC2C                - FFT complex-to-complex transforms for single/double precision
*/
