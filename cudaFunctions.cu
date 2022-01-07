#if 0
#include <cuda_runtime.h>
#include <helper_cuda.h>
#endif

#include <stdio.h>
#include "functions.h"
#define THREADS_PER_BLOCK 256
__global__  void histograma(int *d_Aarray, int size, int* histograma) {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    int histogramSize = size*2*2;
    __shared__ int sharedArrayHistogram [histogramSize];
     sharedArrayHistogram[threadIdx.x] = 0;
    __syncthreads();

    if (id < size)
        atomicAdd(&(sharedArrayHistogram[d_Aarray[id]]), 1);
    __syncthreads();

    atomicAdd(&histograma[threadIdx.x], sharedArrayHistogram[threadIdx.x]);

}

int* createsTheSecondHalfOfArray(int* array, int fullSize){
    int size = fullSize/2;
    int* secondHalfArray= (int*)malloc(size * sizeof(int));
    for(int i=size; i<fullSize ; i++){
        secondHalfArray[i] = array[i];
    }
    return secondHalfArray;
}

// returns 1 if successful, otherwise returns 0
int computeOnGPU(int *arrayOfNumbers, int amountOfNumbersToSlave, int* histograma) {
    //Culcolate the second half of the slave array
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    int size = (amountOfNumbersToSlave/2) * sizeof(int);//size_t
    int* newArrayOfNumbers = createsTheSecondHalfOfArray(arrayOfNumbers, amountOfNumbersToSlave);//the size is size

    // Allocate memory on GPU to copy the data from the host
    int* d_Aarray;
    int* d_Histograma;
    err = cudaMalloc((void **)&d_Aarray, size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }
    err = cudaMalloc((void **)&d_Histograma, amountOfNumbersToSlave*2);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Copy data from host to the GPU memory
    err = cudaMemcpy(d_Aarray, newArrayOfNumbers, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        return 0;
    }
    err = cudaMemcpy(d_Histograma, histograma, amountOfNumbersToSlave*2, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        return 0;
    }


    // Launch the Kernel
    int blocksPerGrid =(size + THREADS_PER_BLOCK) / THREADS_PER_BLOCK;
    histograma<<<blocksPerGrid, THREADS_PER_BLOCK>>>(d_Aarray, size, histograma);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch incrementByOne kernel -  %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Copy the  result from GPU to the host memory.
    err = cudaMemcpy(histograma, d_Histograma, amountOfNumbersToSlave*2, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy result array from device to host -%s\n", cudaGetErrorString(err));
        return 0;
    }

    // Free allocated memory on GPU
    if (cudaFree(d_Aarray) != cudaSuccess) {
        fprintf(stderr, "Failed to free device data - %s\n", cudaGetErrorString(err));
        return 0;
    }

    return 1;
}

