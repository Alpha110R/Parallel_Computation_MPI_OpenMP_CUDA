#pragma once


int computeOnGPU(int *data, int n);
int *readFromFile(const char *fileName, int *numberOfPoints);
void masterCalcHistograma(int* arrayOfNumbers, int amountOfNumbers, int* histograma);
void slaveCalcHistogramaOpenMP(int* arrayOfNumbers, int amountOfNumbers, int* histograma);
int checkIfNumberInRange(int threadID, int range, int number);