#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "functions.h"
#define FILE_NAME "input.txt"
#define MASTER 0
#define SLAVE 1
#define TAG 0


int main(int argc, char *argv[]) {
    int size,
        myRank,
        amountOfNumbers,
        amountOfNumbersToSlave;
    int* data;
    int* arrayOfNumbers;
    int* histograma;
    MPI_Status  status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    if(myRank == MASTER){
       
       if (size != 2) {
       fprintf(stderr, "Run the program with two processes only\n");
       MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
       arrayOfNumbers = readFromFile(FILE_NAME, &amountOfNumbers);
       MPI_Send(&amountOfNumbers, 1, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
       MPI_Send(arrayOfNumbers, amountOfNumbers/2, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
       histograma = (int*)calloc(amountOfNumbers, sizeof(int));
      if(histograma == NULL) {
         printf("Problem to allocate memotry histograma\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
      masterCalcHistograma(arrayOfNumbers, amountOfNumbers, histograma);
    }
    else{
       MPI_Recv(&amountOfNumbers, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       amountOfNumbersToSlave = amountOfNumbers/2;
       arrayOfNumbers = (int*)malloc(amountOfNumbersToSlave * sizeof(int));
       if(arrayOfNumbers == NULL) {
         printf("Problem to allocate memotry arrayOfNumbers\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
       }
      MPI_Recv(arrayOfNumbers, amountOfNumbersToSlave, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      histograma = (int*)calloc(amountOfNumbers, sizeof(int));
      if(histograma == NULL) {
         printf("Problem to allocate memotry histograma\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
      // slaveCalcHistogramaOpenMP(arrayOfNumbers, amountOfNumbersToSlave/2, histograma);
      computeOnGPU(arrayOfNumbers, amountOfNumbersToSlave, histograma);
    }
    
       
    MPI_Finalize();
    free(arrayOfNumbers);
    free(histograma);

    return 0;
}


