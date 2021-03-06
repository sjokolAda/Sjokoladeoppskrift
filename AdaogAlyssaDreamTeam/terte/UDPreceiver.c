#include "UDPreceiver.h"


void* UDP_listener()
{	
	float f =0.0;
	char buf[MSG_LENGTH];
	
	int err = pthread_mutex_init(&tinfo_connection.sendlock, NULL);
	if (err !=0)
	{
		perror("Sendlock init failed");
	}

	while(1){
		if((udp_receive(&tinfo_connection.conn, buf, MSG_LENGTH))==-1)
		{
			perror("Udp_receive failed");
		}

		if (!strcmp("SIGNAL",buf))
		{			
			sem_post(&new_sig);
		}
		else
		{ 
			pthread_mutex_lock(&tinfo_output.lock);
			sscanf(buf, "GET_ACK:%f", &f);
			tinfo_output.y_val = f;				
		
			pthread_mutex_unlock(&tinfo_output.lock);
			sem_post(&y_updt);	
		}					
	}
		
}
		
