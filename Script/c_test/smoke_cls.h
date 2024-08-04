#ifndef _SMOKE_CLS_H
#define _SMOKE_CLS_H

// Define your neural network model
struct SimpleNN {
    double layer1_weights[11][3];
    double layer1_biases[11];
    double layer2_weights[3][11];
    double layer2_biases[3];
};

extern const struct SimpleNN nn;
void print_buf(const float *buf, int len);
int find_max_index(const float *arr, int length);
int find_max_index_int(const int *arr, int length);
int forward(const struct SimpleNN *nn, const float *input, float *output);


#endif