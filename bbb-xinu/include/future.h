#include <futurequeue.h>

#ifndef _FUTURE_H_
#define _FUTURE_H_
 
/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3 

typedef struct futent
{
	int *value;
	int flag;
	int state;         	
	pid32 pid;
	int set_head;
	int set_tail;
	int get_head;
	int get_tail;
	pid32 set_queue[qSize];
	pid32 get_queue[qSize];

} future;


uint32 future_cons(future *fut);
uint32 future_prod(future *fut);

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

uint32 network_cons(future *fut,int32 slot,char *str);
uint32 network_prod(future *fut,int32 slot);
 
#endif /* _FUTURE_H_ */
