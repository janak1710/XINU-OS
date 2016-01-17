/* future_free.c - future_free */

#include <xinu.h>
#include <future.h>

/*------------------------------------------------------------------------
 *  future_free  -  Frees space for a future
 *------------------------------------------------------------------------
 */
syscall	future_free(
	  future* f
	)
{
	intmask mask;
	mask = disable();
	
	syscall sig = freemem((char*)f, sizeof(future));

	restore(mask);
	return sig;
}
