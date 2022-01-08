#if 0
#include <cuda_runtime.h>
#include <helper_cuda.h>
#endif

#include <stdio.h>
#include "functions.h"
#define THREADS_PER_BLOCK 256
#define HISTOGRAMA_SIZE 256

__global__  void histogramaCuda(int *d_Aarray, int size, int* d_Histograma) {
    int id = blockIdx.x * blockDim.x + threadIdx.x;
    __shared__ int sharedArrayHistogram[HISTOGRAMA_SIZE];
     sharedArrayHistogram[threadIdx.x] = 0;
     for(int i=0; i< size; i++){
      d_Histograma[i] = 0;
     }
    __syncthreads();

    if (id < size)
        atomicAdd(&(sharedArrayHistogram[d_Aarray[id]]), 1);
    __syncthreads();

    atomicAdd(&d_Histograma[threadIdx.x], sharedArrayHistogram[threadIdx.x]);
    if(id==0){
        printf("%d\n", d_Histograma[250]);
        printf("%d\n", sharedArrayHistogram[250]);

    }
}

// returns 1 if successful, otherwise returns 0
int computeOnGPU(int *arrayOfNumbers, int amountOfNumbersToSlave, int* histograma) {
    //Culcolate the second half of the slave array
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    int size = (amountOfNumbersToSlave/2);//size_t
    //int* newArrayOfNumbers = createsTheSecondHalfOfArray(arrayOfNumbers, amountOfNumbersToSlave);//the size is size
    int HistogramaFromCuda[HISTOGRAMA_SIZE];
    initializeArray(HistogramaFromCuda, HISTOGRAMA_SIZE);
    // Allocate memory on GPU to copy the data from the host
    int* d_Aarray;
    int* d_Histograma;
    err = cudaMalloc((void **)&d_Aarray, size);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }
    err = cudaMalloc((void **)&d_Histograma, HISTOGRAMA_SIZE);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory - %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Copy data from host to the GPU memory
    err = cudaMemcpy(d_Aarray, arrayOfNumbers, size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data from host to device - %s\n", cudaGetErrorString(err));
        return 0;
    }

    // Launch the Kernel
    int blocksPerGrid =(size + THREADS_PER_BLOCK) / THREADS_PER_BLOCK;
    histogramaCuda<<<blocksPerGrid, THREADS_PER_BLOCK>>>(d_Aarray, size, d_Histograma);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to launch incrementByOne kernel -  %s\n", cudaGetErrorString(err));
        return 0;
    }
    //printf("%d\n", d_Histograma[250]);
    // Copy the  result from GPU to the host memory.
    err = cudaMemcpy(HistogramaFromCuda, d_Histograma, HISTOGRAMA_SIZE, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy result array from device to host -%s\n", cudaGetErrorString(err));
        return 0;
    }

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

