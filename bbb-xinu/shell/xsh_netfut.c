/* xsh_netfut.c - xsh_netfut */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <future.h>

/*----------------------------------------------------------------------
 * xsh_netfut - Communication between client and server using futures
 *----------------------------------------------------------------------
 */

#define BUFFSIZE 1024

shellcmd xsh_netfut(int nargs, char *args[]) {

	char remoteip[]="192.168.1.100";
	char tempbuff[BUFFSIZE];
	//char locip[]="192.168.1.101";

	uint32 remoteipnew;
	uint32 server,client;
	uint32 rsend,rrecv,rip;

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s <name>\n\n", args[0]);
		printf("Description:\n");
		printf("\tEstablishes a communication between\n");
		printf("udp client and server using futures\n");
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	/* Check argument count */
	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
		args[0]);
		return 1;
	}

	if (nargs == 2) {

		future *f_exclusive;	
		f_exclusive = future_alloc(FUTURE_EXCLUSIVE);

		dot2ip(remoteip,&remoteipnew);
		server = udp_register(0,0,1026);
		client = udp_register(remoteipnew,1025,1026);
		resume(create(network_cons, 1024, 20, "fcons1", 3, f_exclusive,client,args[1]));
  		resume(create(network_prod, 1024, 20, "fprod1", 3, f_exclusive,server) );
		
		return 0;
	} else {
		fprintf(stderr,"Invalid argument.\n Try %s --help for more information\n",args[0]);
		return 1;
	}

	return 0;

}
