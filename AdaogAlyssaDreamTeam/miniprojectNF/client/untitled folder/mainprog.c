#include <arpa/inet.h>
#include "shared.h"
#include "network.h"
#include "PIDctrl.h"
#include "signalHandler.h"
void init_sem_and_mutexes();
void destroy_sem_and_mutexes();
int main()
{
	int err=0;
	
	
	//init mutexes and semaphores
	init_sem_and_mutexes();		
	// Init UDP connection struct
	err = udp_init_client(&tinfo_connection.conn, UDP_PORT, SERVER_IP);
	if (err!=0)
	{
		printf("error connecting to server");
	}
	printf("created udp conn\n");
	/*
	//init shared output struct
	tinfo_output_t y;
	y.y_val = 0;
	*/
	//err = pthread_mutex_init(&tinfo_output.lock, NULL);
	//if (err !=0)
	//{
	//	printf("error initializing mutex");
	//}

		



    	//Start threads
	
	pthread_t udprecv, pidctrl, sighandl;

	err = pthread_create(&udprecv, NULL, UDP_listener, NULL);
	if (err!=0)
	{
		printf("Thread Creation failed\n");
	}
	printf("created udp listener");

	err = pthread_create(&pidctrl, NULL, PIDctrl, NULL);
	if (err!=0)
	{
		perror("Thread Creation failed\n");
	}
	
	err = pthread_create(&sighandl, NULL,signalHandler,NULL );
	if (err!=0)
	{
		perror("Thread Creation failed\n");
	}	
	
	

	// start simulation

	pthread_mutex_lock(&tinfo_connection.sendlock);
	err = udp_send(&tinfo_connection.conn, "START", sizeof("START"));
	if (err==-1){
		printf("Failed to start simulation\n");	
	}
	pthread_mutex_unlock(&tinfo_connection.sendlock);

	printf("started simulation\n");
	//join thread
	

	err = pthread_join(pidctrl, NULL);  
    	if (err != 0) {  
        	printf("Joining of thread failed stooped\n");    
    	}
	
	// stop simulation
	pthread_mutex_lock(&tinfo_connection.sendlock);
	err = udp_send(&tinfo_connection.conn, "STOP", sizeof("STOP"));
	if (err==-1){
		printf("Failed to stop simulation\n");	
	}
	pthread_mutex_unlock(&tinfo_connection.sendlock);
	
	//destroy_sem_and_mutexes();
	return 0;      
}


void init_sem_and_mutexes(){
	//Init semaphores/mutexes
	int err1 = sem_init(&new_sig,0,0);
	int err2 = sem_init(&y_updt,0,0);
	if ((err1|| err2)!=0)
	{
		perror("Sem init failed");
	}
	int errc= pthread_mutex_init(&tinfo_connection.sendlock,NULL);
	int erro = pthread_mutex_init(&tinfo_output.lock, NULL);	
	if ((errc||erro)!=0)
	{
		perror("Mutex init failed");
	}

}

void destroy_sem_and_mutexes(){

	//Destroy all semaphores and mutexes
	int err1= sem_destroy(&new_sig);
	int err2= sem_destroy(&y_updt);
	int err3= pthread_mutex_destroy(&tinfo_connection.sendlock);
	int err4= pthread_mutex_destroy(&tinfo_output.lock);
	if( (err1 || err2 || err3 || err4)!=0)
	{
		perror("Could not destroy the sem or mutex");
	}
}

