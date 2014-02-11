/*
 * boundedBuffer.c
 *
 * A complete example of simple producer/consumer program. The Producer
 * and Consumer functions are executed as independent threads.  They
 * share access to a single buffer, data.  The producer deposits a sequence
 * of integers from 1 to numIters into the buffer.  The Consumer fetches
 * these values and adds them.  Two semaphores,empty and full are used to
 * ensure that the producer and consumer alternate access to the buffer.
 *
 * SOURCE: adapted from example code in "Multithreaded, Parallel, and
 *         Distributed Programming" by Gregory R. Andrews.
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SHARED 1

void *Producer (void *); // the two threads
void *Consumer (void *);

sem_t empty, full, mutex;       //global semaphores
int *data;                // shared buffer, size = 1
int numIters;
int bufferSize;
int currentIndex = 0;
int currentReadIndex = 0;

// main() -- read command line and create threads
int main(int argc, char *argv[]) {
    pthread_t pid[3];
	pthread_t cid[3];

    if (argc < 3) {
	    printf("Usage: boundedBuffer <Number of Iterations> <bufer size>\n");
	    printf(" e.g.: ./boundedBuffer 100 \n"); 
	    exit(0);
    }
    numIters = atoi(argv[1]);
	bufferSize = atoi(argv[2]);
	data = new int[bufferSize];

    sem_init(&empty, SHARED, bufferSize);    // sem empty = 1
    sem_init(&full, SHARED, 0); //sem full = 0
	sem_init(&mutex, SHARED, 1); //initialize mutex to allow 1 thread access

	pthread_create(&(pid[1]), NULL, Producer, NULL);
	pthread_create(&(pid[2]), NULL, Producer, NULL);
	pthread_create(&(pid[3]), NULL, Producer, NULL);

	pthread_create(&(cid[1]), NULL, Consumer, NULL);
	pthread_create(&(cid[2]), NULL, Consumer, NULL);
	pthread_create(&(cid[3]), NULL, Consumer, NULL);

	pthread_join(pid[1], NULL);
	pthread_join(cid[1], NULL);

	pthread_join(pid[2], NULL);
	pthread_join(cid[2], NULL);

	pthread_join(pid[3], NULL);
	pthread_join(cid[3], NULL);

    pthread_exit(0);
}

// deposit 1, ..., numIters into the data buffer
void *Producer(void *arg) {
    int produced;

    for (produced = 0; produced < numIters; produced++) {
		//Locking to allow only 10 producers max
        sem_wait(&empty);
		//Locking to allow only 1 thread access to data array
		sem_wait(&mutex);

        data[currentIndex++ % bufferSize] = produced;

		//Free mutex, allowing another thread access to data array
		sem_post(&mutex);
		//Signal to allow one more consumer
        sem_post(&full);
    }
}

//fetch numIters items from the buffer and sum them
void *Consumer(void *arg) {
    int total = 0;
    int consumed;

    for (consumed = 0; consumed < numIters; consumed++) {
		//Locking to allow only 10 consumers max
        sem_wait(&full);
		//Locking to allow only 1 thread access to data array
		sem_wait(&mutex);

        total = total + data[currentReadIndex++ % bufferSize];

		//Free mutex, allow another thread access to the data array
		sem_post(&mutex);
		//Signal to allow one more producer
        sem_post(&empty);
    }

    printf("the total is %d\n", total);
}
