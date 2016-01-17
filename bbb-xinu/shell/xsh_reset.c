/* xsh_reset.c - xsh_reset */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_reset - 
 *------------------------------------------------------------------------
 */
shellcmd xsh_reset(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tResets the XINU\n");
		return 0;
	}

	/* Check argument count */

	if (nargs == 1) {
		volatile uint32 *prm_rstctrl = (volatile uint32 *) 0x44E00F00;
		*prm_rstctrl = 0x01;
	} 

	return 0;
}
