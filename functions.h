#pragma once


int computeOnGPU(int *data, int n);
int *readFromFile(const char *fileName, int *numberOfPoints);
void masterCalcHistograma(int* arrayOfNumbers, int amountOfNumbers, int* histograma);
