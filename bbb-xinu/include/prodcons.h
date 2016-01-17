#include <xinu.h>  
#include <stdio.h>

/*Global variable for producer consumer*/
extern int n;

/* Declare the required semaphores */
extern sid32 consumed, produced; 

/*function Prototype*/
void consumer(int count);
void producer(int count);

