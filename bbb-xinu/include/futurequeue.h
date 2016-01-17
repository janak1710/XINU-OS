#ifndef _MYQUEUE_H_
#define _MYQUEUE_H_

#include <kernel.h>
#include <stdbool.h>

#define qSize 10    

void enQueue(pid32 queue[],pid32 pid, int * head, int * tail);
pid32 deQueue(pid32 Q[],int *head, int *tail);
bool isQueueEmpty(int * head, int * tail);
bool IsQueueFull(int* head, int* tail);

#endif
