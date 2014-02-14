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

///
//Checks if it is safe for a car to cross going the given direction
///
//Parameters:
//direc: The new vehicles direction
///
//Output:
//True is safe to cross; otherwise, false
///
bool isSafe(int direc)
{
	bool safe = false;

	//always safe when bridge is empty
	if(currentNumber==0)
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

	return safe;
}

///
//Handles vehicle arriving at the bridge.
///
//Parameters
//direc: The direction the current vehicle is traveling
//id: the "Id" of the current vehicle
///
void ArriveBridge(int direc, int id)
{
	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);

	//Check if safe to cross the bridge
	while(!isSafe(direc))
	{
		//if not safe, wait.
		pthread_cond_wait(&safe, &currentNumber_mutex);
	}

	printf("Car %d traveling in direction %d has arrived at the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );

	//increment currentNumber
	currentNumber++;
	currentDirec = direc;

	//MUTEX UNLOCK
	pthread_mutex_unlock(&currentNumber_mutex);

	sched_yield();
	return;
}

///
//Handles a vehicle crossing the bridge
///
//Parameters
//direc: The direction the current vehicle is traveling
//id: The "Id" of the current vehicle
///
void CrossBridge(int direc, int id)
{
	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);

	//crossing the bridge
	printf("Car %d traveling in direction %d is crossing the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );

	//MUTEX UNLOCK
	pthread_mutex_unlock(&currentNumber_mutex);

	sched_yield();
	return;
}

///
//Handles a vehicle exiting the bridge
///
//Parameter
//direc: The direction the current vehicle is traveling
//id: The "Id" of the current vehicle
/// 
void ExitBridge(int direc, int id)
{
	//MUTEX LOCK
	pthread_mutex_lock(&currentNumber_mutex);

	//Decrease the number of vehicles currently on the bridge.
	currentNumber--;

	//printf("Car %d traveling in direction %d has exited the bridge.\n", id, direc);
	printf("Car %d traveling in direction %d has exited the bridge. CurrDir: %d. #Cars: %d. \n", id, direc, currentDirec, currentNumber );

	//Broadcast to safe condition, that another vehicle can attempt
	//to cross the bridge.
	pthread_cond_broadcast(&safe);

	//MUTEX UNLOCK
	pthread_mutex_unlock(&currentNumber_mutex);
	sched_yield();
	return;
}

///
//The vehicle thread, simulates a vehicle crossing the bridge.
///
//Parameters
//idv: The vehicle/thread id.
/// 
void *OneVehicle(void *idv)
{
	int id = *((int*)(&idv));

	//Generate a random direction
	int direc = rand() % 2;

	ArriveBridge(direc, id);
	CrossBridge(direc, id);
	ExitBridge(direc, id);

	pthread_exit(NULL);
}

///
//The main thread for the application
///
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

	//Seed the random number generator
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
