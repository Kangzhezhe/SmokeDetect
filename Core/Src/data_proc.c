#include "data_proc.h"
#include <string.h> // for memcpy
#include <stdlib.h> // for qsort

int filterIndex = 0;
double delta_LED_R_filter[FILTER_SIZE];
double delta_LED_B_filter[FILTER_SIZE];
double R_B_ratio_filter[FILTER_SIZE];

int compareDouble(const void* a, const void* b);

double medianFilter(double value, double* array) {
  array[filterIndex] = value;
  filterIndex = (filterIndex + 1) % FILTER_SIZE;
  
  double sortedArray[FILTER_SIZE];
  memcpy(sortedArray, array, sizeof(sortedArray));
  qsort(sortedArray, FILTER_SIZE, sizeof(double), compareDouble);
  
  double median;
  if (FILTER_SIZE % 2 == 0) {
    median = (sortedArray[FILTER_SIZE/2 - 1] + sortedArray[FILTER_SIZE/2]) / 2;
  } else {
    median = sortedArray[FILTER_SIZE/2];
  }
  // Return the median value as the filtered value
  return median;
}

int compareDouble(const void* a, const void* b) {
  double fa = *(const double*)a;
  double fb = *(const double*)b;
  
  if (fa < fb) {
    return -1;
  } else if (fa > fb) {
    return 1;
  } else {
    return 0;
  }
}

