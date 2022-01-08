#pragma once


int computeOnGPU(int *data, int n);
void masterCalcHistograma(int* arrayOfNumbers, int amountOfNumbers, int* histograma);
void slaveCalcHistogramaOpenMP(int* arrayOfNumbers, int amountOfNumbers, int* histograma);
int checkIfNumberInRange(int threadID, int range, int number);
int computeOnGPU(int *arrayOfNumbers, int amountOfNumbersToSlave, int* histograma);
void mergeHistograms(int* histograma, int* histogramaToMerge);
void initializeArray(int* array, int size);