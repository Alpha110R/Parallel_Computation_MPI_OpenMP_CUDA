#if 0
#include <cuda_runtime.h>
#include <helper_cuda.h>
#endif

#include <stdio.h>
#include "functions.h"
#define THREADS_PER_BLOCK 256
#define HISTOGRAMA_SIZE 256

__global__  void histogramaCuda(int *d_Array, int quarterOfFullSize, int* d_Histograma) {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    __shared__ int sharedArrayHistogram[HISTOGRAMA_SIZE];
     sharedArrayHistogram[threadIdx.x] = 0;
    __syncthreads();

    if (id < quarterOfFullSize)
        atomicAdd(&(sharedArrayHistogram[d_Array[id]]), 1);
    __syncthreads();
    atomicAdd(&(d_Histograma[threadIdx.x]), sharedArrayHistogram[threadIdx.x]);
}

// returns 1 if successful, otherwise returns 0
int computeOnGPU(int *arrayOfNumbers, int quarterOfFullSize, int* histograma, int* cudaHistograma) {
    //Culcolate the second half of the slave array
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    // Allocate memory on GPU to copy the data from the host
    int* d_Aarray;
    int* d_Histograma;
    err = cudaMalloc((void **)&d_Aarray, quarterOfFullSize * sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }
    err = cudaMalloc((void **)&d_Histograma, HISTOGRAMA_SIZE * sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }
    cudaMemset(d_Histograma, 0, HISTOGRAMA_SIZE * sizeof(int));

    // Copy data from host to the GPU memory
    err = cudaMemcpy(d_Aarray, arrayOfNumbers, quarterOfFullSize * sizeof(int), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Launch the Kernel
    int blocksPerGrid =(quarterOfFullSize + THREADS_PER_BLOCK) / THREADS_PER_BLOCK;
    histogramaCuda<<<blocksPerGrid, THREADS_PER_BLOCK>>>(d_Aarray, quarterOfFullSize, d_Histograma);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch incrementByOne kernel -  %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Copy the  result from GPU to the host memory.
    err = cudaMemcpy(cudaHistograma, d_Histograma, HISTOGRAMA_SIZE * sizeof(int), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy result array from device to host -%s\n", cudaGetErrorString(err));
        return 0;
    }
    mergeHistograms(histograma, cudaHistograma);
    // Free allocated memory on GPU
    if (cudaFree(d_Aarray) != cudaSuccess) {
        fprintf(stderr, "Failed to free device data - %s\n", cudaGetErrorString(err));
        return 0;
    }
    if (cudaFree(d_Histograma) != cudaSuccess) {
        fprintf(stderr, "Failed to free device data - %s\n", cudaGetErrorString(err));
        return 0;
    }

    return 1;
}

