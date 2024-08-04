#ifndef DATA_PROC_H
#define DATA_PROC_H

#define FILTER_SIZE 5
extern double delta_LED_R_filter[FILTER_SIZE];
extern double delta_LED_B_filter[FILTER_SIZE];
extern double R_B_ratio_filter[FILTER_SIZE];


double medianFilter(double currentValue, double* array) ;

#endif


