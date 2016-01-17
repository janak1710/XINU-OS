/* printfreemem.c - printfreemem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  printfreemem  -  Print Free Memory list
 *------------------------------------------------------------------------
 */
syscall	printfreemem()
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	struct	memblk*	ptr = &memlist;

	if(ptr == NULL) {
		kprintf("\nMemory Full");
		restore(mask);
		return SYSERR;
	}
	while(ptr != NULL) {
		kprintf("\tFree Size - %u, Free Memory Location - %d\n",ptr->mlength, ptr->mnext);
		ptr = ptr->mnext;	
	}

	restore(mask);
	return OK;
}
