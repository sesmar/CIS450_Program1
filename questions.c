#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define SHARED 1

void *Student(void *);
void *Professor(void *);

//Declare global semaphores
sem_t questionStart, answerStart, answerDone;

int main(int argc, char *argv[])
{
	if (argc < 2) 
	{
		printf("Usage: questions <Number of Students>\n");
		printf(" e.g.: ./questions 50 \n");
		exit(0);
	}

	int numStudents = atoi(argv[1]);

	pthread_attr_t attr;
	pthread_t professor;
	pthread_t studentThreads[numStudents];

	sem_init(&questionStart, SHARED, 1);
	sem_init(&answerStart, SHARED, 0);
	sem_init(&answerDone, SHARED, 0);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&professor, &attr, Professor, NULL);

	for(int i = 0; i < numStudents; i++)
	{
		pthread_create(&studentThreads[i], &attr, Student, (void*)(i + 1));
	}

	pthread_join(professor, NULL);

	for(int i = 0; i < numStudents; i++)
	{
		pthread_join(studentThreads[i], NULL);
	}

	pthread_exit(0);
}

void *Professor(void *pId)
{
	while(true)
	{
		sem_wait(&answerStart);

		printf("Professor is answering the question.\n");

		sem_post(&answerDone);
	}
}

void *Student(void *pId)
{
	int studentId = *((int*)(&pId));

	printf("Student %d is waiting to ask a question.\n", studentId);

	sem_wait(&questionStart);

	printf("Student %d is asking a question.\n", studentId);

	sem_post(&answerStart);
	sem_wait(&answerDone);

	printf("Student %d is leaving.\n", studentId);

	sem_post(&questionStart);
}
