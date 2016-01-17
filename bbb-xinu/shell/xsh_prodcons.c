/* xsh_prodcons.c - xsh_prodcons */
#include <prodcons.h>
#include <future.h>

/*------------------------------------------------------------------------
 * xsh_prodcons - Producer Consumer Problem
 *------------------------------------------------------------------------
 */

int n;                 //Definition for global variable 'n'
sid32 consumed, produced; 

shellcmd xsh_prodcons(int nargs, char *args[])
{
	
	int count = 0;
	//Argument verifications and validations

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays Producer Consumer Problem\n");
		printf("\tAccepts 1 optional integer argument\n");
		printf("\tDefault Integer argument = 2000\n");
		return 0;
	} else if (nargs == 2 && strncmp(args[1], "-f", 3) == 0) {
		
		future *f_queue;
		future *f_exclusive;
		future *f_shared;
		f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
		f_shared = future_alloc(FUTURE_SHARED);
		f_queue = future_alloc(FUTURE_QUEUE);
 
		//Test FUTURE_EXCLUSIVE
		resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
		resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );

		// Test FUTURE_SHARED
		resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
		resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
		resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
		resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
		resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );

		// Test FUTURE_QUEUE

		resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
		resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
		resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
		resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
		resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
		resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );

	} else if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	} else if (nargs == 1) {
		count = 2000;
	} else {
 		char *s = args[1];
		int i;
		for(i = 0; i<strlen(s); i++) {
			if(s[i] >= '0' && s[i] <='9') {
				count *= 10;
				count += s[i] - '0'; 
			} else {
				fprintf(stderr, "%s: invalid arguments\n", args[0]);
				fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
				return 1;
			} 

		}
	}
	consumed = semcreate(0);      
      	produced = semcreate(1);

      	//create the process producer and consumer and put them in ready queue.
      	//Look at the definations of function create and resume in exinu/system folder for reference.      
      	resume( create(producer, 1024, 20, "producer", 1, count) );
      	resume( create(consumer, 1024, 20, "consumer", 1, count) );
}
