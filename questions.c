//CIS 450
//Program 1 - The Professor and Students Problem
//Chris Sims and Michael Ingrody

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define SHARED 1


//Declare functions
void *Student(void *);
void *Professor(void *);
void QuestionStart(int studentId);
void QuestionFinish(int studentId);
void AnswerStart();
void AnswerFinish();

//Declare global semaphores
sem_t questionStart, answerStart, answerDone;

int main(int argc, char *argv[])
{
	//Error for insufficient parameter
	if (argc < 2) 
	{
		printf("Usage: questions <Number of Students>\n");
		printf(" e.g.: ./questions 50 \n");
		exit(0);
	}

	int numStudents = atoi(argv[1]);   //Main() parameter is the number of students

	pthread_attr_t attr;

	//Initialize threads for professor and students
	pthread_t professor;                     
	pthread_t studentThreads[numStudents];   

	//Initialize semaphores
	sem_init(&questionStart, SHARED, 1);    
	sem_init(&answerStart, SHARED, 0);
	sem_init(&answerDone, SHARED, 0);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//Create professor thread
	pthread_create(&professor, &attr, Professor, (void*)numStudents);

	//Create student threads
	for(int i = 0; i < numStudents; i++)
	{
		pthread_create(&studentThreads[i], &attr, Student, (void*)(i + 1));
	}

	//Join professor thread to main()
	pthread_join(professor, NULL);

	//Join all student threads to main()
	for(int i = 0; i < numStudents; i++)
	{
		pthread_join(studentThreads[i], NULL);
	}

	//TODO: relocate to preserve modularity
	printf("All student questions have been answered.\n");   

	//end main()
	pthread_exit(0);  

}

//Professor thread
void *Professor(void *numStudents)
{
	//set thread id
	int i = *((int*)(&numStudents));
	
	//while there are students with questions
	for(i; i>0; i--)
	{
		printf("The professor wants to be asked a question.\n");
		AnswerStart();

		printf("Professor is answering the question.\n");
		
		AnswerFinish();
	}
}

//Student thread
void *Student(void *pId)
{
	//set thread id
	int studentId = *((int*)(&pId));

	QuestionStart(studentId);

	printf("Student %d is asking a question.\n", studentId);
	printf("Student %d is done asking a question.\n", studentId);
	
	sem_post(&answerStart);     //tell professor thread to answer

	QuestionFinish(studentId);

	sched_yield();
}

void QuestionStart(int studentId)
{
	printf("Student %d is waiting to ask a question.\n", studentId);

	sem_wait(&questionStart);    //wait for opportunity to ask question

	sched_yield();
}

void QuestionFinish(int studentId)
{
	sem_wait(&answerDone);      //wait until professor thread finishes answering question

	printf("Student %d received the answer and is leaving.\n", studentId);

	sem_post(&questionStart);   //allow opportunity for the next student to ask a question

	sched_yield();
}

void AnswerStart()
{
	sem_wait(&answerStart);     //wait until student has finished asking the question
	sched_yield();
}

void AnswerFinish()
{
	printf("The professor is finished with the answer.\n");	

	sem_post(&answerDone);     //tell student thread the answer is finished
	sched_yield();
}
