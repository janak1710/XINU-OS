/* kill.c - kill */

#include <xinu.h>
#include <testproc.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	
	/* Count Max Stack size */	

	uint32 maxSize = 0;
	uint32 ssize = prptr->prstklen;
	uint32 *bsptr = prptr->prstkbase;
	while(ssize > 0){
		if(*bsptr-- == -1)
			maxSize++;
		ssize--;
	}

	//kprintf("\nStack Size By Subtracting Base and STK PTR - %u",(prptr->prstkbase - prptr->prstkptr)/sizeof(char*));
	//kprintf("\nMax Stack Size - %u",prptr->prstklen - maxSize);

	if(!skipFreeStk)
		freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
