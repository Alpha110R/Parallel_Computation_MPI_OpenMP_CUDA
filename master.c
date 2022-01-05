#include "functions.h"
#include <stdio.h>
#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void masterCalcHistograma(int* arrayOfNumbers, int amountOfNumbers, int* histograma){
#pragma omp parallel for default(none) shared(arrayOfNumbers) private(histograma) firstprivate(amountOfNumbers)//The master calculate the second half of the array
    for(int i=amountOfNumbers/2 ; i< amountOfNumbers; i++){
        int number = arrayOfNumbers[i];
        histograma[number]++;
    }

for(int i=0 ; i< amountOfNumbers;i++){
    printf("histograma: %d", histograma[i]);
}

}
