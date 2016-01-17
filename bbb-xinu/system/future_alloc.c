/* future_alloc.c - future_alloc */

#include<xinu.h>
#include <future.h>

/*------------------------------------------------------------------------
 *  future_alloc  -  Allocates space for a new future
 *------------------------------------------------------------------------
 */
future*	future_alloc(
	  int future_flag
	)
{
	intmask mask;
	mask = disable();
	
	future * x = (future *)getmem(sizeof(future));	
	x->flag = future_flag;
	x->state = FUTURE_EMPTY;
	x->value = NULL;
	x->pid = NULLPROC;
	x->set_head = -1;
	x->set_tail = -1;	
	
	x->get_head = -1;
	x->get_tail = -1;

	restore(mask);
	return x;
}
