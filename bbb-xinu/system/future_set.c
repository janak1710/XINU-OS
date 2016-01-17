/* future_set.c - future_set */

#include<xinu.h>
#include <future.h>
/*------------------------------------------------------------------------
 *  future_set
 *------------------------------------------------------------------------
 */
syscall	future_set(future* f, int* value)
{
	if(f->flag == FUTURE_EXCLUSIVE) { 
		if(f->state ==FUTURE_VALID)
		{
			return SYSERR;
		}	
		if((f->state == FUTURE_WAITING || f->state == FUTURE_EMPTY))
		{
			f->state = FUTURE_VALID;
			f->value=*value;	
			return OK;
		}
	} else if(f->flag == FUTURE_QUEUE) {
		pid32 pid;
                intmask mask = disable();
		if(isQueueEmpty(&(f->get_head),&(f->get_tail))) {
			f->value=*value;
			enQueue(f->set_queue, getpid(), &(f->set_head), &(f->set_tail));
                       	suspend(getpid());
		}else{
			
			f->value=*value;
			pid = deQueue(f->get_queue, &(f->get_head), &(f->get_tail));
			resume(pid);
			
		}
		restore(mask);
		return OK;
			
	}else if(f->flag == FUTURE_SHARED) {
		intmask mask = disable();
		pid32 pid;
		if(f->state == FUTURE_EMPTY) {
			f->state = FUTURE_VALID;
			f->value = *value;
			while(!isQueueEmpty(&(f->get_head),&(f->get_tail))) {
				pid = deQueue(f->get_queue, &(f->get_head), &(f->get_tail));
        	                resume(pid);
			}
		} else if(f->state == FUTURE_VALID) {
			restore(mask);
			return SYSERR;
		}
		restore(mask);
		return OK;
	
	}	
	
return SYSERR;
}
