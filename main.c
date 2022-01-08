#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "functions.h"
#include "readWriteFunctions.h"
#define FILE_NAME "input.txt"
#define MASTER 0
#define SLAVE 1
#define TAG 0
#define HISTOGRAMA_SIZE 256


void slaveCalcHistogramaOpenMP(int* arrayOfNumbers, int amountOfNumbers, int* histograma){
    //The slave calculate the first half of the array and the OpenMP the first quarter of his array
#pragma omp parallel default(none) shared(arrayOfNumbers) shared(histograma) firstprivate(amountOfNumbers)
{
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
    int* arrayOfNumbers;
    int histograma[HISTOGRAMA_SIZE]={0};
    int histogramaFromSlave[HISTOGRAMA_SIZE];
    int* tryhistograma;//Array for sequantial histograma to test the parallel
    MPI_Status  status;
    FILE* histograma_file;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    if(myRank == MASTER){
       if (size != 2) {
       fprintf(stderr, "Run the program with two processes only\n");
       MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
      }
      histograma_file = fopen("histograma.txt","w");
      if(!histograma_file){
         printf("Error writing to file");
      }
       arrayOfNumbers = readFromFile(FILE_NAME, &amountOfNumbers);
       MPI_Send(&amountOfNumbers, 1, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
       MPI_Send(arrayOfNumbers, amountOfNumbers/2, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
       //tryhistograma = (int*)calloc(HISTOGRAMA_SIZE, sizeof(int));
      masterCalcHistograma(arrayOfNumbers, amountOfNumbers, histograma);
      MPI_Recv(histogramaFromSlave, HISTOGRAMA_SIZE, MPI_INT, SLAVE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      mergeHistograms(histograma, histogramaFromSlave);
      printSnakeIdRectangle(histograma, HISTOGRAMA_SIZE, histograma_file);
      //////////////////////////////////////////
      /*
      for(int i=0; i<HISTOGRAMA_SIZE;i++){
         printf("i: %d  number: %d\n", i, histograma[i]);
      }
      for(int i=0; i<amountOfNumbers;i++){
         tryhistograma[arrayOfNumbers[i]]++;
      }
      printf("SEQUANTIAL ###########");
      for(int i=0; i<HISTOGRAMA_SIZE; i++){
         printf("i: %d  number: %d\n", i, tryhistograma[i]);
      }*/
      /////////////////////////////////////////
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
      int quarterOfFullSize = amountOfNumbersToSlave/2;
      slaveCalcHistogramaOpenMP(arrayOfNumbers, quarterOfFullSize, histograma);
      computeOnGPU(arrayOfNumbers + (quarterOfFullSize), quarterOfFullSize, histograma);//sends the second half of the array
      MPI_Send(histograma, HISTOGRAMA_SIZE, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
    }
    
       
    MPI_Finalize();
    free(arrayOfNumbers);
    //free(histograma);
    

    return 0;
}

void mergeHistograms(int* histograma, int* histogramaToMerge){
   for(int i=0; i<HISTOGRAMA_SIZE; i++){
      histograma[i]+=histogramaToMerge[i];
   }
}

void initializeArray(int* array, int size){
   for(int i=0; i< size; i++){
      array[i] = 0;
   }
}