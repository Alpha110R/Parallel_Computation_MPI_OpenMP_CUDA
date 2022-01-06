#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "functions.h"
#define FILE_NAME "input.txt"
#define MASTER 0
#define SLAVE 1
#define TAG 0

void slaveCalcHistogramaOpenMP(int* arrayOfNumbers, int amountOfNumbers, int* histograma){
    //The slave calculate the first half of the array and the OpenMP the first of the half of the half
#pragma omp parallel default(none) shared(arrayOfNumbers) shared(histograma) firstprivate(amountOfNumbers)
{//The master calculate the second half of the array
    int threadID,
        numberOfThreads,
        range,
        counter=0;
    threadID = omp_get_thread_num();
    numberOfThreads = omp_get_num_threads();
    range = amountOfNumbers / numberOfThreads;
    for(int i= 0; i< amountOfNumbers; i++){
        if(checkIfNumberInRange(threadID, range, arrayOfNumbers[i]) == 1){
            histograma[arrayOfNumbers[i]]++;   
        }
    }
}
}

int checkIfNumberInRange(int threadID, int range, int number){
    int lastNumberInTheRange,
        differenceRangeNumberToCheck;
    lastNumberInTheRange = (range * threadID) + range -1;
    differenceRangeNumberToCheck = lastNumberInTheRange - number;
    if(differenceRangeNumberToCheck > range || differenceRangeNumberToCheck <0)
        return 0;
   return 1;
}


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
         printf("Problem to allocate memotry arraOfNumbers\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
       }
      MPI_Recv(arrayOfNumbers, amountOfNumbersToSlave, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      histograma = (int*)calloc(amountOfNumbers, sizeof(int));
      if(histograma == NULL) {
         printf("Problem to allocate memotry histograma\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
      slaveCalcHistogramaOpenMP(arrayOfNumbers, amountOfNumbersToSlave/2, histograma);
    }
    
       
    MPI_Finalize();
    free(arrayOfNumbers);
    free(histograma);

    return 0;
}


