#include "functions.h"
#include <stdio.h>

// Reads a number of numbers from the file.
// The first line contains the amount of numbers defined.
int *readFromFile(const char *fileName, int *amountOfNumbers) {
    FILE* fp;
    int* arrayNumber;
// Open file for reading points
    if ((fp = fopen(fileName, "r")) == 0) {
        printf("cannot open file %s for reading\n", fileName);
        exit(0);
    }
// Amount of numbers
    fscanf(fp, "%d", amountOfNumbers);
// Allocate array and Read data from the file
    arrayNumber = (int*)malloc(2 * *amountOfNumbers  * sizeof(int));
    if (arrayNumber == NULL) {
        printf("Problem to allocate memotry\n");

        exit(0);
    }
    for (int i = 0; i < *amountOfNumbers; i++) {
        fscanf(fp, "%d %d", &arrayNumber[2*i], &arrayNumber[2*i + 1]);
    }
    fclose(fp);
    return arrayNumber;
}