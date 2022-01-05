#pragma once

#define PART  100


int computeOnGPU(int *data, int n);
int *readFromFile(const char *fileName, int *numberOfPoints);
