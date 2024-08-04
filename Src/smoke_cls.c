#include <stdio.h>
#include <math.h>
#include "smoke_cls.h"

float norms[3] = {1.8929253e+05, 8.7214867e+04, 6.8633751e+01};

const struct SimpleNN nn = {
    {{-7.83740758895874, -8.848414421081543, -12.24759292602539},
     {5.292802333831787, 4.515924453735352, -13.517144203186035},
     {-0.10120275616645813, 0.3429213762283325, -0.32338947057724},
     {0.193948090076, -0.085551917552, -0.315690040587},
     {0.19074875116348267, -0.18679052591323853, 0.1589878797531128},
     {0.2807830572128296, 0.20381444692611694, -0.5668717622756958},
     {-0.34320881962776184, 0.18666332960128784, 0.5028241872787476},
     {13.835766792297363, 12.963648796081543, -9.918196678161621},
     {-6.330789566040039, -5.897296905517578, -11.507247924804688},
     {-0.09985899925231934, -0.5554795861244202, 0.364618718624115},
     {0.505520224571228, -0.21995773911476135, -0.22039857506752014}},
    {0.47243380546569824, 0.7736398577690125, -0.16537627577781677,
     -0.1935487687587738, -0.4783748984336853, -0.43683063983917236,
     -0.3663662075996399, 0.5511552691459656, 0.426161527633667,
     -0.12146195769309998, -0.22127306461334229},
    {{-7.560817241668701, 1.3656795024871826, 0.2862808108329773,
      -0.27796393632888, 0.16969498991966248, -0.05976878106594086,
      0.07565325498580933, 9.625694274902344, -6.394925594329834,
      0.06414636969566345, -0.16001464426517487},
     {8.209916114807129, 5.920668601989746, 0.05427497625350952,
      -0.24351124465465546, -0.25075048208236694, 0.25596606731414795,
      -0.23066475987434387, -1.0018506050109863, 5.4514360427856445,
      0.21294474601745605, -0.12958915531635284},
     {-6.561015605926514, -10.504884719848633, 0.08915695548057556,
      0.10387581586837769, 0.10856708884239197, -0.008997499942779541,
      -0.12122142314910889, -11.737171173095703, -1.202599048614502,
      0.10549050569534302, -0.037905871868133545}},
    {-0.8819454908370972, -3.9943816661834717, 5.057967662811279}};

// Function to normalize a 2D array
void normalize(const float data_combined[3], float *output) {
    // Calculate the L2 norm along dim=0
    for (int i = 0; i < 3; i++) {
        output[i] = data_combined[i] / norms[i];
    }
}

// Define the ReLU activation function
float relu(float x) { return fmaxf(0, x); }

// Define the Softmax activation function
void softmax(float *x, int length) {
    float max_val = x[0];
    for (int i = 1; i < length; i++) {
        if (x[i] > max_val) {
            max_val = x[i];
        }
    }
    float sum = 0;
    for (int i = 0; i < length; i++) {
        x[i] = expf(x[i] - max_val);
        sum += x[i];
    }
    for (int i = 0; i < length; i++) {
        x[i] /= sum;
    }
}

void print_buf(const float *buf, int len) {
    for (int i = 0; i < len; i++) {
        printf("%f ", buf[i]);
    }
    printf("\n");
}

int find_max_index(const float *arr, int length) {
    int max_index = 0;
    float max_val = arr[0];

    for (int i = 1; i < length; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
            max_index = i;
        }
    }

    return max_index;
}

int find_max_index_int(const int *arr, int length) {
    int max_index = 0;
    int max_val = arr[0];

    for (int i = 1; i < length; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
            max_index = i;
        }
    }

    return max_index;
}

// Define the forward pass function
int forward(const struct SimpleNN *nn, const float *input, float *output) {
    float normed[3];
    // print_buf(input, 3);
    normalize(input, normed);
    // print_buf(normed, 3);
    // Perform the forward pass through the network
    float layer1_output[11];
    for (int i = 0; i < 11; i++) {
        layer1_output[i] = 0;
        for (int j = 0; j < 3; j++) {
            layer1_output[i] += normed[j] * nn->layer1_weights[i][j];
        }
        layer1_output[i] += nn->layer1_biases[i];
        layer1_output[i] = relu(layer1_output[i]);
    }
    for (int i = 0; i < 3; i++) {
        output[i] = 0;
        for (int j = 0; j < 11; j++) {
            output[i] += layer1_output[j] * nn->layer2_weights[i][j];
        }
        output[i] += nn->layer2_biases[i];
    }
    softmax(output, 3);
    return find_max_index(output, 3);
}
