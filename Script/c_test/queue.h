#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdio.h>
#include <stdbool.h>

#define Q_SIZE 5

bool isFull();
bool isEmpty() ;
void enqueue(int value) ;
int dequeue() ;
int countelement(int element);

#endif