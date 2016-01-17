#include <xinu.h>
#include <futurequeue.h>
#include <stdbool.h>


void enQueue(pid32 Q[],pid32 pid, int *head, int *tail)
{		
	if(IsQueueFull(head, tail)){
		return SYSERR;
	}
	if (isQueueEmpty(head, tail)){ 
		*head = *tail = 0; 
	}else{
		*tail = (*tail+1);
	}
	Q[*tail] = pid;
}


pid32 deQueue(pid32 Q[], int *head, int *tail)
{
	pid32 x;
	if(isQueueEmpty(head, tail)){
		return SYSERR;
	} else if(*head == *tail){
		x = Q[*head];
		*tail = *head = -1;
	} else {
		x = Q[*head];
		*head = (*head+1);
	}
	return x;
}

bool IsQueueFull(int* head, int* tail)
{
	return (*tail+1) == qSize ? true : false;
}

bool isQueueEmpty(int* head, int* tail)
{
	return (*head == -1 && *tail == -1); 
}

