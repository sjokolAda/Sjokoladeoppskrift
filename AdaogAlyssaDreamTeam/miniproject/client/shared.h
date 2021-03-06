
#ifndef SHARED_H_
#define SHARED_H_

#include "miniproject.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>


//Server contants
#define UDP_PORT 9999
#define SERVER_IP "129.241.187.146"
#define MSG_LENGTH 255


//Semaphoresused for signaling
sem_t y_updt;
sem_t new_sig;


//Output struct
typedef struct
{
	double y_val;
	pthread_mutex_t lock;	
}tinfo_output_t;



//Connection struct  
typedef struct
{
	struct udp_conn conn;
	pthread_mutex_t sendlock;
}tinfo_connection_t;

tinfo_output_t tinfo_output;
tinfo_connection_t tinfo_connection;



#endif /*SHARED_H_*/
