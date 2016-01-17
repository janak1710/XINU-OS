/* future_get.c - future_get */

#include <xinu.h>
#include <future.h>
 
syscall future_get(future *f, int* value){
	if(f->flag == FUTURE_EXCLUSIVE) {
		if(f->state == FUTURE_EMPTY)
	        {
			f->state = FUTURE_WAITING;
			f->pid = getpid();
			while(1) {
				intmask mask = disable();
				kprintf("");
				restore(mask);
				if(f->state==FUTURE_VALID) {
					f->state=FUTURE_EMPTY;
					*value=f->value;
					return OK;
				}
			}
		}
              
	        if(f->state == FUTURE_VALID) {
			intmask mask = disable();                                             
			kprintf("");
			f->state=FUTURE_EMPTY;                       
	               *value=f->value;
	               restore(mask);
	               return OK;    
	        }
 	} else if(f->flag == FUTURE_QUEUE) {

		intmask mask = disable();
		pid32 tpid;
		if(isQueueEmpty(&(f->set_head),&(f->set_tail))){
			enQueue(f->get_queue,getpid(),&(f->get_head),&(f->get_tail));
			suspend(getpid());
			*value = f->value;
		}else{
			tpid = deQueue(f->set_queue, &(f->set_head), &(f->set_tail));
			resume(tpid);
			*value = f->value;

		}
		restore(mask);
		return OK;
	} else if(f->flag == FUTURE_SHARED) {
	
		intmask mask = disable();
		if(f->state == FUTURE_EMPTY) {
			enQueue(f->get_queue,getpid(),&(f->get_head),&(f->get_tail));
			suspend(getpid());
		}
		if(f->state == FUTURE_VALID) {
			*value = f->value;
		}
	
		restore(mask);
		return OK;
	
	}
return SYSERR;
}
