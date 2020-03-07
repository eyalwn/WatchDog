/*******************************************************************************
*	Filename :		task_test.c
*	Developer :		Eyal Weizman
*	Last Update :	20 Feb 2019
*	Description :	task test file
*******************************************************************************/
#include <stdio.h> 		/* printf */
#include <time.h>		/* time */
#include <unistd.h>		/* sleep */

#include "task.h"


/*** MACROS ***/
#define UNUSED(x) ((void) x)


/*** internal functions ***/
void TaskIntegratedTest(void);
int TaskRetDone(void * data);
int TaskRetRepeat(void * data);
int TaskRetFail(void * data);


/*****************************************************************************
*								main
******************************************************************************/
int main(void)
{
	printf("\n***** INTEGRATED TEST FOR TASK FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");

	TaskIntegratedTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	return (0);
}


/******************************************************************************
*								Integrated-Test
*******************************************************************************/
void TaskIntegratedTest(void)
{
	task_t *task_1 = NULL;
	task_t *task_2 = NULL;
	task_t *task_3 = NULL;
	
	task_1 = TaskCreate(TaskRetDone, NULL, time(NULL) + 3, 10);
	task_2 = TaskCreate(TaskRetRepeat, NULL, time(NULL) + 6, 10);
	task_3 = TaskCreate(TaskRetFail, NULL, time(NULL) + 7, 10);
	
	printf("TaskCreate:\t\t\t");
	(NULL != task_1) ?
	printf("SUCCESS") : printf("FAIL");
	printf("\n\n--------------------------------------------------------\n\n");
	
	printf("TaskGetId:\t\t\t");
	(0 == UIDIsBad(TaskGetId(task_1))) ? /* expected: not bad UID */
	printf("SUCCESS") : printf("FAIL");
	printf("\n\n--------------------------------------------------------\n\n");
	
	printf("TaskGetRunTime:\t\t\t");
	(time(NULL) + 3 == TaskGetRunTime(task_1)) ?
	printf("SUCCESS") : printf("FAIL");
	printf("\n\n--------------------------------------------------------\n\n");
	
	/* waits 4 sec to check if the run_time updated */
	printf("please wait 4 seconds...\n\n");
	sleep(4);
	/* updates run_time */
	TaskUpdateRunTime(task_1);
	
	/* now lets check if it worked */
	printf("TaskGetRunTime after update:\t");
	(time(NULL) + 10 == TaskGetRunTime(task_1))
	?
	printf("SUCCESS") : printf("FAIL");
	printf("\n\n--------------------------------------------------------\n\n");
	
	/* run checking */
	printf("TaskRun: returned values + excecutions of all 3 functions\n\n");
	(DONE 	== TaskRun(task_1)) &&
	(REPEAT	== TaskRun(task_2)) &&
	(FAIL 	== TaskRun(task_3))
	?
	printf("\nret values:\t\t\tSUCCESS") : printf("ret values:\t\t\tFAIL");
	printf("\n\n--------------------------------------------------------\n\n");
	
	TaskDestroy(task_1);
	TaskDestroy(task_2);
	TaskDestroy(task_3);
	
	printf("PQDestroy:\t\t\tSUCCESS (RUN WITH VALGRIND)");
}


/******************************************************************************
*								Task-functions
*******************************************************************************/

/****************************** TaskRetDone ***********************************/
int TaskRetDone(void * data)
{
	UNUSED(data);
	
	printf("TaskRetDone: this function returns DONE.\n");
	
	return(DONE);
}


/****************************** TaskRetRepeat *********************************/
int TaskRetRepeat(void * data)
{
	UNUSED(data);
	
	printf("TaskRetRepeat: this function returns REPEAT.\n");
	
	return(REPEAT);
}


/****************************** TaskRetFail ***********************************/
int TaskRetFail(void * data)
{
	UNUSED(data);
	
	printf("TaskRetFail: this function returns FAIL.\n");
	
	return(FAIL);
}




