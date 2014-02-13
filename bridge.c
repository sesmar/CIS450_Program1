#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_THREADS 50
#define safeCapacity 3;

pthread_mutex_t currentNumber_mutex;
pthread_cond_t safe;

int currentNumber = 0;
int currentDirec = 0;

//lock for current direction or direction?
bool isSafe(int direc)
{
	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);
	bool safe = false;

	if(currentNumber==0)  //always safe when bridge is empty
	{
		safe = true;;
	}
	// room for us to follow in the same direction
	else if ((currentNumber < 3) && (currentDirec == direc))
	{
		safe = true;
	}
	//bridge if full or has oncoming traffic
	else
	{
		safe = false;
	}

	pthread_mutex_unlock(&currentNumber_mutex);
	return safe;
}

void ArriveBridge(int direc, int id)
{
	printf("Car %d arrive start, direction: %d\n", id, direc);
	while(!isSafe(direc))
	{
		pthread_cond_wait(&safe, &currentNumber_mutex);
	}

	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);

	//increment currentNumber
	currentNumber++;

	//MUTEX UNLOCK
	pthread_mutex_unlock(&currentNumber_mutex);

	currentDirec = direc;
	
	//printf("Car %d traveling in direction %d has arrived at the bridge.\n", id, direc);
	printf("Car %d traveling in direction %d has arrived at the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );
	sched_yield();
	return;

}

void CrossBridge(int direc, int id)
{
	//crossing the bridge
	printf("Car %d traveling in direction %d is crossing the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );
	sched_yield();
	return;
}

void ExitBridge(int direc, int id)
{
	printf("Car %d exit start\n", id);
	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);

	currentNumber--;

	//MUTEX UNLOCK
	pthread_mutex_unlock(&currentNumber_mutex);

	pthread_cond_signal(&safe);
	//printf("Car %d traveling in direction %d has exited the bridge.\n", id, direc);
	printf("Car %d traveling in direction %d has exited the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );
	sched_yield();
	return;
}

void *OneVehicle(void *idv)
{
	int id = (int) idv;
	int direc = rand() % 2;

	ArriveBridge(direc, id);
	CrossBridge(direc, id);
	ExitBridge(direc, id);

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i;
	//Create 50 threads;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;

	/*Initialize mutex and conditional variable objects*/
	pthread_mutex_init (&currentNumber_mutex, NULL);
	pthread_cond_init (&safe, NULL);

	/*For portability, create threads in a joinable state*/
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	srand(time(NULL));

	for(i=0; i<NUM_THREADS; i++)
	{
		pthread_create(&threads[i], &attr, OneVehicle, (void*)i);
	}

	for(i=0; i<NUM_THREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	printf("Main(): Waited on %d threads.  Done.\n", NUM_THREADS);

	/*Clean up and exit*/
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&currentNumber_mutex);
	pthread_cond_destroy(&safe);
	pthread_exit (NULL);
}
