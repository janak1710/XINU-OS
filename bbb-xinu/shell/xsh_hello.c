/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_hello - obtain and print the entered string along with hello and 
 * Welcome to the world of Xinu
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays hello along with the entered string and\n");
		printf("\tWelcome to the world of Xinu\n");
		printf("\tAccepts only 1 character string without special characters\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	} else if(nargs < 2) {
		fprintf(stderr, "%s: too few arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	} else {
		char *s = args[1];
		int i;
		for(i =0; i<strlen(s); i++) {
			if(!((s[i] >='a' && s[i] <='z') || (s[i] >='A' && s[i] <='Z')))  {
				fprintf(stderr, "%s: invalid arguments\n", args[0]);
				fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
				return 1;
			}
		}
		
		printf("Hello %s, Welcome to the world of Xinu!!\n", args[1]);
		
	}

	return 0;
}
