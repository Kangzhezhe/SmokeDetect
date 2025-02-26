#include "queue.h"
#include "smoke_cls.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


#define N 100
float inputs[N][3] = {
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {346.0, 94.0, 3.6808510246717976},
    {1.0, 94.0, 3.6808510246717976},
    {2.0, 94.0, 3.6808510246717976},
    {0.0, 1.0, 0.0},
    {289.0, 84.0, 3.4404761495181413},
    {337.0, 93.0, 3.623655875014453},
    {371.0, 101.0, 3.6732672903636905},
    {382.0, 105.0, 3.6380952034467122},
    {0.0, 1.0, 0.0},
    {348.0, 93.0, 3.7419354436351027},
    {275.0, 77.0, 3.571428525046383},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {1542.0, 848.0, 1.8183962242707592},
    {2105.0, 962.0, 2.1881496858751044},
    {1633.0, 733.0, 2.2278308291571203},
    {1798.0, 946.0, 1.9006342474623317},
    {0.0, 1.0, 0.0},
    {6397.0, 3477.0, 1.8398044285764152},
    {25440.0, 10521.0, 2.418021100426003},
    {24315.0, 9867.0, 2.464274855329454},
    {17121.0, 7420.0, 2.3074123986108606},
    {0.0, 1.0, 0.0},
    {21025.0, 9340.0, 2.251070663570549},
    {26995.0, 12815.0, 2.1065158016303926},
    {11717.0, 6019.0, 1.9466688815506448},
    {8775.0, 4488.0, 1.9552139033076616},
    {28225.0, 13297.0, 2.1226592462869323},
    {27481.0, 13461.0, 2.0415273752290672},
    {0.0, 1.0, 0.0},
    {20682.0, 9844.0, 2.1009752131144883},
    {0.0, 1.0, 0.0},
    {30371.0, 13012.0, 2.3340762371400188},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {20373.0, 8957.0, 2.274533883859045},
    {12264.0, 5419.0, 2.2631481819038295},
    {18135.0, 6627.0, 2.7365323671742066},
    {11016.0, 3556.0, 3.0978627662829408},
    {5190.0, 1692.0, 3.0673758847119528},
    {1927.0, 502.0, 3.838645410679989},
    {1028.0, 346.0, 2.9710982573089644},
    {1046.0, 415.0, 2.520481921637393},
    {1049.0, 475.0, 2.2084210479822715},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {1043.0, 522.0, 1.9980842873599918},
    {1228.0, 567.0, 2.165784828631773},
    {900.0, 405.0, 2.2222222167352537},
    {630.0, 282.0, 2.234042545269353},
};

int state = 1;
int smoke_cls(float inputs[3]) {
    float output[3];
    float input[3];
    memcpy(input, inputs, 3 * sizeof(float));
    int max_index = forward(&nn, input, output);
    // printf("Predicted class: %d\n", max_index);
    enqueue(max_index);
    // printf("count of 0: %d\n", countelement(0));
    // printf("count of 1: %d\n", countelement(1));
    // printf("count of 2: %d\n", countelement(2));
    int arr[3] = {countelement(0), countelement(1), countelement(2)};
    // printf("max index: %d\n",find_max_index_int(arr,3));
    if (state == 1) {
        if (find_max_index_int(arr, 3) == 0) {
            state = 0;
        } else if (find_max_index_int(arr, 3) == 2) {
            state = 2;
        }
    } else if (state == 0) {
        if (countelement(0) == 0)
            state = 1;
    } else if (state == 2) {
        if (countelement(2) == 0)
            state = 1;
    }
    dequeue();
    printf("state: %d\n", state);
    return  state;
}

int main() {
    for (int i = 0; i < Q_SIZE - 1; i++) {
        enqueue(1);
    }
    
    for (int i = 0; i < N; i++) {
        if (inputs[i][0] == 0.0 && inputs[i][1] == 0.0 && inputs[i][2] == 0.0) {
            break;
        }
        int s = smoke_cls(inputs[i]);
    }
    // // Perform a forward pass through the neural network
    // int max_index = forward(&nn, input, output);
    // printf("Predicted class: %d\n", max_index);

    // // Print the output probabilities
    // printf("Output probabilities: %f, %f, %f\n", output[0], output[1],
    //        output[2]);

    printf("Press any key to exit...\n");
    getchar();
    return 0;
}
