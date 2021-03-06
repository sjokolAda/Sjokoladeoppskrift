
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define PHILOS 5

void *philosopher(void *id);
pthread_t philo[PHILOS];

void grab_fork(int, int, char *);
void putdown_fork(int, int);

pthread_mutex_t fork_lock[PHILOS];

int start = 0;

int main()
{

	int i;
	for (i = 0; i < PHILOS; i++)
    	{
		pthread_mutex_init (&fork_lock[i], NULL);
		if (pthread_mutex_init(&fork_lock[i], NULL) != 0)
		{
        		perror("mutex initilization");
			exit(0);
   		}
	}
	for (i=0;i<PHILOS;i++){
		
		pthread_create (&philo[i], NULL, philosopher, (void *)i);
	}
	
	start = 1;
	for(i=0; i<PHILOS;i++)
	{
		pthread_join (philo[i], NULL);
	}
	return 0;
}

void *philosopher(void *num)
{
	int id;
	int left_fork, right_fork;

	id = (int)num;
	printf("philosopher %d is done thinking and wats food\n", id);
	right_fork = id;
	left_fork = id+1;
	// Wrap around the chopsticks. */
		if (left_fork == PHILOS)
	{
		left_fork = 0;
	}
	grab_fork(id, right_fork, "right ");
	grab_fork(id, left_fork, "left");
	 printf ("Philosopher %d: eating.\n", id);
	putdown_fork (left_fork, right_fork);

	printf ("Philosopher %d is done eating.\n", id);
	 return (NULL);
	
}

void grab_fork(int phil, int c, char *hand)
{
	pthread_mutex_lock(&fork_lock[c]);
	printf ("Philosopher %d: got %s fork %d\n", phil, hand, c);
}

void putdown_fork(int c1, int c2)
{
	pthread_mutex_unlock(&fork_lock[c1]);
	pthread_mutex_unlock(&fork_lock[c2]);
}



