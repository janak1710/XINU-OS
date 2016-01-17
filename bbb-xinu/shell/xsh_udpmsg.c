/* xsh_udpmsg.c - xsh_udpmsg */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*----------------------------------------------------------------------
 * xsh_udpmsg - Establishes a communication between udp client and server
 *----------------------------------------------------------------------
 */

#define BUFFSIZE 1024

shellcmd xsh_udpmsg(int nargs, char *args[]) {

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
		printf("udp client and server\n");
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
		dot2ip(remoteip,&remoteipnew);
		server = udp_register(0,0,1026);
		client = udp_register(remoteipnew,1025,1026);
		
		rsend = udp_send(client,args[1],strlen(args[1]));

		if(rsend==SYSERR)
			printf("Error\n");
		else{
			printf("Message Sent : %s\n",args[1]);
		}

		rrecv = udp_recv(server,tempbuff,BUFFSIZE,15000);
		
		printf("Waiting to receive data\n");
		
		if(rrecv==SYSERR)
			printf("Error\n");
		else if(rrecv==TIMEOUT)
			printf("Timeout\n");
		else{
			
			printf("Message Received : %s\n", tempbuff);
		}
		
		return 0;
	} else {
		fprintf(stderr,"Invalid argument.\n Try %s --help for more information\n",args[0]);
		return 1;
	}

	return 0;

}
