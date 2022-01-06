#include "functions.h"
#include <stdio.h>
#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void slaveCalcHistogramaOpenMP(int* arrayOfNumbers, int amountOfNumbers, int* histograma){
    //The slave calculate the first half of the array and the OpenMP the first of the half of the half
#pragma omp parallel default(none) shared(arrayOfNumbers) shared(histograma) firstprivate(amountOfNumbers)
{//The master calculate the second half of the array
    int threadID,
        numberOfThreads,
        range;
    threadID = omp_get_thread_num();
    numberOfThreads = omp_get_num_threads();
    range = amountOfNumbers / numberOfThreads;
    #pragma omp for
    for(int i= range*threadID; i< amountOfNumbers/2; i++){
        if(checkIfNumberInRange(threadID, range, arrayOfNumbers[i]) == 1)
            histograma[i]++;
    }
    printf("thread #%d histograma: count:%d\n",omp_get_thread_num(), histograma[5]);

}
}

int checkIfNumberInRange(int threadID, int range, int number){
    int lastNumberInTheRange,
        differenceRangeNumberToCheck;
    lastNumberInTheRange = range * threadID + range -1;
    differenceRangeNumberToCheck = lastNumberInTheRange - number;
    if(differenceRangeNumberToCheck >= range || differenceRangeNumberToCheck <0)
        return 0;
    return 1;
}