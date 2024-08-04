#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdio.h>
#include <stdbool.h>

#define Q_SIZE 5

bool isFull(void);
bool isEmpty(void) ;
void enqueue(int value) ;
int dequeue(void) ;
int countelement(int element);

#endif