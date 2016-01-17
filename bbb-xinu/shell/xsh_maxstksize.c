/* xsh_maxstksize.c - xsh_maxstksize */

#include <xinu.h>
#include <testproc.h>

/*------------------------------------------------------------------------
 * xsh_maxstksize - prints the max stk size
 *------------------------------------------------------------------------
 */
bool skipFreeStk;

shellcmd xsh_maxstksize(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tPrints the max stk size\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	} else {
		skipFreeStk = true;
		resume( create(testproc, 1024, 20, "testproc", 1, 1000) );
	}

	return 0;
}
