/*******************************************************************************
*	Filename :		task.c
*	Developer :		Eyal Weizman
*	Last Update :	21 Feb 2019
*	Description:	task source file
*******************************************************************************/
#include <stdlib.h>		/* malloc, free */
#include <assert.h> 	/* assert */
#include <string.h> 	/* memset */
#include <time.h>		/* time */

#include "task.h"


/*** structures ***/
struct task
{
	unique_id_t id;
	time_t run_time;
	time_t interval;
	int(*task_func)(void *data);
	void *data;
};


/******************************************************************************
*								TaskCreate
*******************************************************************************/
task_t *TaskCreate(task_func_t task_func, void * data,
		  		   time_t start_time, time_t interval)
{
	task_t *new_task = NULL;
	
	assert(task_func);
	
	new_task = (task_t *) malloc(sizeof(task_t));
	if (NULL == new_task)
	{
		return (NULL);
	}
	
	new_task->id = UIDCreate();
	new_task->run_time = start_time;
	new_task->interval = interval;
	new_task->task_func = task_func;
	new_task->data = data;
	
	return (new_task);
}


/******************************************************************************
*								TaskDestroy
*******************************************************************************/
void TaskDestroy(task_t *task)
{
	assert(task);
	
	memset(task, 0, sizeof(task_t));
	free(task);
	task = NULL;
}


/******************************************************************************
*								TaskGetUid
*******************************************************************************/
unique_id_t TaskGetId(task_t *task)
{
	assert(task);
	
	return (task->id);
}


/******************************************************************************
*								TaskGetRunTime
*******************************************************************************/
time_t TaskGetRunTime(task_t *task)
{
	assert(task);
	
	return (task->run_time);
}


/******************************************************************************
*								TaskUpdateRunTime
*******************************************************************************/
void TaskUpdateRunTime(task_t *task)
{
	assert(task);
	
	task->run_time = time(NULL) + task->interval;
}


/******************************************************************************
*								TaskRun
*******************************************************************************/
int TaskRun(task_t *task)
{
	assert(task);
	
	return (task->task_func(task->data));
}


















