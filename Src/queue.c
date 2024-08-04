#include <stdio.h>
#include <stdbool.h>
#include "queue.h"


int queue[Q_SIZE];
int front = -1, rear = -1;

bool isFull() {
    if ((rear + 1) % Q_SIZE == front) {
        return true;
    }
    return false;
}

bool isEmpty() {
    if (front == -1) {
        return true;
    }
    return false;
}

void enqueue(int value) {
    if (isFull()) {
        printf("Queue is full\n");
    } else {
        if (front == -1) {
            front = 0;
        }
        rear = (rear + 1) % Q_SIZE;
        queue[rear] = value;
        // printf("%d enqueued to queue\n", value);
    }
}

int dequeue() {
    if (isEmpty()) {
        printf("Queue is empty\n");
        return -1;
    } else {
        int item = queue[front];
        if (front == rear) {
            front = rear = -1; // Reset front and rear when the queue becomes empty
        } else {
            front = (front + 1) % Q_SIZE;
        }
        return item;
    }
}

int countelement(int element) {
    int count = 0;
    for (int i = front; i != -1; i = (i + 1) % Q_SIZE) {
        if (queue[i] == element) {
            count++;
        }
        if (i == rear) {
            break; // Stop counting after reaching the last element in the queue
        }
    }
    return count;
}

// int main() {
//     enqueue(10);
//     enqueue(20);
//     enqueue(20);
//     enqueue(10);
//     enqueue(10);
//     printf("count of 10: %d\n", count(10));
//     printf("count of 20: %d\n", count(20));
//     printf("count of 30: %d\n", count(30));
//     printf("count of 0: %d\n", count(0));
//     printf("count of 50: %d\n", count(50));

//     printf("Dequeued: %d\n", dequeue());

//     enqueue(60);
//     enqueue(70);
//     enqueue(80);
//     printf("Dequeued: %d\n", dequeue());
//     printf("Dequeued: %d\n", dequeue());
//     printf("Dequeued: %d\n", dequeue());
//     printf("Dequeued: %d\n", dequeue());

//     getchar();
//     return 0;
// }