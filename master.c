#include "functions.h"
#include <stdio.h>
#include <mpi.h>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void masterCalcHistograma(int* arrayOfNumbers, int amountOfNumbers, int* histograma){

#pragma omp parallel default(none) shared(arrayOfNumbers) shared(histograma) firstprivate(amountOfNumbers)
{//The master calculate the second half of the array
    int* privateHistograma = (int*)calloc(amountOfNumbers,sizeof(int));
    #pragma omp for
    for(int i=amountOfNumbers/2 ; i< amountOfNumbers; i++){
        int number = arrayOfNumbers[i];
        ++privateHistograma[number];
        histograma[number]++;
        //printf("thread #%d number: %d\n",omp_get_thread_num(),number);
        //printf("thread #%d histograma: number: %d count:%d\n",omp_get_thread_num(), number, privateHistograma[number]);
    }
   //printf("thread #%d histograma: count:%d\n",omp_get_thread_num(), privateHistograma[0]);

}
}
