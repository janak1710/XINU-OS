#include <future.h>
#include <xinu.h>

uint32 network_cons(future *fut,int32 slot,char *str) {
	int i, status;
	udp_send(slot,str,strlen(str));
	status = future_get(fut, &i);
	if (status < 1) {
		kprintf("future_get failed\n");
    		return SYSERR;
 	} else {
		printf("Message set :%d\n", i);
	}
	return OK;
}


uint32 network_prod(future *fut,int32 slot) {
	int j,status;
	char tempbuff[1024];
	int32 msglen = udp_recv(slot,tempbuff,1024,15000);
	if (msglen == -1){
		printf("TimeOut!!\n");
		return TIMEOUT;
	}else{
		printf("Message Received :%s\n",tempbuff);	
		int i;	
		for(i = 0; i<strlen(tempbuff); i++) {
			j = j + tempbuff[i];
			
		}
  		status = future_set(fut, &j);
  		if (status < 1) {
  			kprintf("future_set failed\n");
    			return SYSERR;
  		}
  		return OK;
	}
}
