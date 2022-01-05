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
        sizeArrayHistograma;
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
       sizeArrayHistograma = amountOfNumbers/2;
       MPI_Send(&sizeArrayHistograma, 1, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD);
       MPI_Send(arrayOfNumbers, sizeArrayHistograma, MPI_INT, SLAVE, TAG, MPI_COMM_WORLD)
    }
    else{
       MPI_Recv(&sizeArrayHistograma, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       arrayOfNumbers = (int*)malloc(sizeArrayHistograma * sizeof(int));
       if(arrayOfNumbers == NULL) {
         printf("Problem to allocate memotry arraOfNumbers\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
       }
      MPI_Recv(arrayOfNumbers, sizeArrayHistograma, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }
    histograma = (int*)malloc(sizeArrayHistograma* sizeof(int));
    if(histgorama == NULL) {
         printf("Problem to allocate memotry histograma\n");
         MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
       }
       
    MPI_Finalize();

    return 0;
}


