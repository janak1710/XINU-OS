/* xsh_printfreemem.c - xsh_printfreemem */

#include <xinu.h>

/*------------------------------------------------------------------------
 * xsh_printfreemem - print free memory address and size
 *------------------------------------------------------------------------
 */

shellcmd xsh_printfreemem(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tPrints free memory address and size\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	} else {
		printfreemem();
		
	}

	return 0;
}
