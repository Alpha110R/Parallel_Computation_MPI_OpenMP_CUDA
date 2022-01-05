#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "functions.h"


int main(int argc, char *argv[]) {
    int size, rank, i;
    int *data;
    MPI_Status  status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 2) {
       fprintf(stderr, "Run the example with two processes only\n");
       MPI_Abort(MPI_COMM_WORLD, __LINE__);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    
       
    MPI_Finalize();

    return 0;
}


