/*******************************************************************************
*	Filename :		scheduler_test.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-02-24
*	Description :	scheduler test file
*******************************************************************************/
#include <stdio.h> 		/* printf */
#include <time.h>		/* time */

#include "scheduler.h"


/*** MACROS ***/
#define UNUSED(x) ((void) x)


/*** unit-test functions ***/
void SchedulerCreateTest(void);
void SchedulerIsEmptyTest(void);
void SchedulerAddTaskTest(void);
void SchedulerSizeTest(void);
void SchedulerRemoveTaskTest(void);
void SchedulerRunTest(void);

/*** task functions ***/
int TaskCountDown(void * data);
int TaskStop(void * data);
int TaskEternal(void * data);
int TaskFail(void * data);

/*****************************************************************************
*								main
******************************************************************************/
int main(void)
{
	printf("\n***** TEST FOR SCHEDULER FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");
	
	SchedulerCreateTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	printf("SchedulerDestroy:\t\t\tRUN VALGRIND");
	printf("\n\n--------------------------------------------------------\n\n");
		
	SchedulerIsEmptyTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	SchedulerAddTaskTest();
	printf("\n\n--------------------------------------------------------\n\n");
		
	SchedulerSizeTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	SchedulerRemoveTaskTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	SchedulerRunTest();
	printf("\n\n--------------------------------------------------------\n\n");
		
	return (0);
}


/******************************************************************************
*								Unit-Tests
*******************************************************************************/

/************************* SchedulerCreateTest ********************************/
void SchedulerCreateTest(void)
{
	scheduler_t *sch_1 = NULL;
	
	printf("SchedulerCreate:\t\t\t");
	sch_1 = SchedulerCreate();
	
	
	(NULL != sch_1)
	?
	printf("SUCCESS") : printf("FAIL");

	SchedulerDestroy(sch_1);
}


/************************* SchedulerIsEmptyTest *******************************/
void SchedulerIsEmptyTest(void)
{
	scheduler_t *sch_1 = NULL;
	
	printf("SchedulerIsEmpty:\t\t\t");
	sch_1 = SchedulerCreate();
	
	(TRUE == SchedulerIsEmpty(sch_1))
	?
	printf("SUCCESS") : printf("FAIL");

	SchedulerDestroy(sch_1);
}


/************************* SchedulerAddTaskTest *******************************/
void SchedulerAddTaskTest(void)
{
	scheduler_t *sch_1 = NULL;
	unique_id_t id_task = {0};
	
	printf("SchedulerAddTask:\t\t\t");
	sch_1 = SchedulerCreate();
	
	id_task = SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL) + 2, 3);
	
	(FALSE == SchedulerIsEmpty(sch_1))	&&
	(FALSE == UIDIsBad(id_task))
	?
	printf("SUCCESS") : printf("FAIL");

	SchedulerDestroy(sch_1);
}


/************************* SchedulerSizeTest **********************************/
void SchedulerSizeTest(void)
{
	scheduler_t *sch_1 = NULL;
	size_t size_when_empty = 0;
	size_t size_when_2_tasks = 0;
	
	printf("SchedulerSize:\t\t\t\t");
	
	sch_1 = SchedulerCreate();
	size_when_empty = SchedulerSize(sch_1);
	
	SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL) + 2, 3);
	SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL) + 2, 3);
	size_when_2_tasks = SchedulerSize(sch_1);
	
	(0 == size_when_empty)	&&
	(2 == size_when_2_tasks)
	?
	printf("SUCCESS") : printf("FAIL");

	SchedulerDestroy(sch_1);
}


/************************* SchedulerRemoveTaskTest ****************************/
void SchedulerRemoveTaskTest(void)
{
	scheduler_t *sch_1 = NULL;
	int ret_val_1 = 0;
	int ret_val_2 = 0;
	size_t size_after_remove_1 = 0;
	size_t size_after_remove_2 = 0;
	unique_id_t id_task_1 = {0};
	
	printf("SchedulerRemoveTask:\t\t\t");

	sch_1 = SchedulerCreate();
	id_task_1 = SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL) + 2, 3);
	SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL) + 2, 3);
	
	/* remove a valid ID */
	ret_val_1 = SchedulerRemoveTask(sch_1, id_task_1);	/* expected: SUCCESS */
	size_after_remove_1 = SchedulerSize(sch_1);			/* expected: 1 */
	
	/* try to remove an invalid ID */
	ret_val_2 = SchedulerRemoveTask(sch_1, id_task_1);	/* expected: FAILURE */
	size_after_remove_2 = SchedulerSize(sch_1);			/* expected: 1 */
	
	(SUCCESS == ret_val_1)		&&
	(FAILURE == ret_val_2)		&&
	(1 == size_after_remove_1)	&&
	(1 == size_after_remove_2)
	?
	printf("SUCCESS") : printf("FAIL");

	SchedulerDestroy(sch_1);
}


/************************* SchedulerRunTest ***********************************/
void SchedulerRunTest(void)
{
	scheduler_t *sch_1 = NULL;
	unique_id_t id_task_eternal = {0};
	int ret_val_run = 0;
	
	printf("\t\tSchedulerRun integrated test:\n\n");
	
	/* run-test 1 */	
	printf("\n\n\t*** tSchedulerRun test 1 ***\n\n");
	sch_1 = SchedulerCreate();
	SchedulerAddTask(sch_1, TaskCountDown, NULL, time(NULL), 5);
	SchedulerAddTask(sch_1, TaskStop, sch_1, time(NULL) + 10, 1);
	id_task_eternal =
			SchedulerAddTask(sch_1, TaskEternal, NULL, time(NULL) + 1, 5);
	
	ret_val_run = SchedulerRun(sch_1);
	
	printf("size-check after-run 1:\t\t\t");
	(2 == SchedulerSize(sch_1))
	?
	printf("SUCCESS") : printf("FAIL");
	
	printf("\nret value check from run 1:\t\t");
	(STOP == ret_val_run)
	?
	printf("SUCCESS") : printf("FAIL");
	
	
	/* run-test 2 */
	printf("\n\n\n\t*** tSchedulerRun test 2 (with 1 failure): ***\n\n");
	SchedulerRemoveTask(sch_1, id_task_eternal);
	SchedulerAddTask(sch_1, TaskFail, NULL, time(NULL) + 4, 4);
	
	ret_val_run = SchedulerRun(sch_1);
	
	printf("\n check after run 2:\t\t");
	(COMPLETE == ret_val_run) 		&&
	(TRUE == SchedulerIsEmpty(sch_1))
	?
	printf("SUCCESS") : printf("FAIL");
	
	SchedulerDestroy(sch_1);
}


/******************************************************************************
*								Task-functions
*******************************************************************************/

/****************************** TaskCountDown *********************************/
int TaskCountDown(void * data)
{
	static int counter = 4;
	
	UNUSED(data);
	
	--counter;
	
	if (counter > 0)
	{
		printf("TaskCountDown: %d rounds had left.\n", counter);
		return (REPEAT);
	}
	else
	{
		printf("TaskCountDown: DONE\n");
		return (DONE);
	}
}


/****************************** TaskStop **************************************/
int TaskStop(void *sch_1)
{
	printf("TaskStop: time to stop now.\n");
	SchedulerStop(sch_1);
	
	return (DONE);
}


/****************************** TaskEternal ***********************************/
int TaskEternal(void * data)
{
	UNUSED(data);
	
	printf("TaskEternal: this funciton is eternal... \n");
	return (REPEAT);
}


/****************************** TaskFail **************************************/
int TaskFail(void * data)
{
	UNUSED(data);
	
	printf("TaskFail: this funciton should fail.... \n");
	return (FAIL);
}
