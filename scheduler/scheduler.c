/*******************************************************************************
*	Filename :		scheduler.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-02-24
*	Description:	source file of a single-threaded scheduler
*******************************************************************************/
#include <stdlib.h>	/* malloc, free */
#include <assert.h> /* assert */
#include <time.h>   /* time */
#include <unistd.h>	/* sleep */
#include <stdio.h> 	/* fprintf */

#include "./pqueue/pqueue.h"
#include "./task/task.h"
#include "scheduler.h"

/***************************** MACROS *****************************************/
#define UNUSED(x) ((void) x)


/***************************** structures *************************************/
struct scheduler
{
	pqueue_t *queue;	/*	pointer to a priority queue */
	int is_running;		/*	a flag witch determines whether the scheduler
						 	runs/stops */
};

/**************************** local functions *********************************/


/*	Description: sort tasks from the smallest to the biggest values of time_t.
 *	compares the run time of a task allready in the list vs. this of a new task
 *	to be placed in the list.
 *	
 *	Argument 1: a pointer to a task allready in the list.
 *	Argument 2: a pointer to a new task to be placed in the list.
 *
 *	Used in function: PQCreate (inside funciton SchedulerCreate);
 */
static int HasHigherPriority(void *queue_tasks, const void *new_task, void *param);


/*	Description: this function checks whether 2 IDs are identical or not.
 *	
 *	Argument 1: a pointer to a task from the queue. the ID of this task will be
 *				compared with the 2nd argument.
 *	Argument 2: a pointer to the received ID from the user to compare with.
 *
 *	Used in function: PQErase (inside funciton SchedulerRemoveTask);
 */
static int IDIsMatch(void *task_in_queue, void *ptr_id_to_check);

/******************************************************************************
*								SchedulerCreate
*******************************************************************************/
scheduler_t *SchedulerCreate(void)
{
	scheduler_t *new_sched = NULL;
	pqueue_t *new_pqueue = NULL;
	
	new_sched = (scheduler_t *) malloc(sizeof(scheduler_t));
	if (NULL != new_sched)
	{
		new_pqueue = PQCreate(HasHigherPriority);
		if (NULL != new_pqueue)
		{
			new_sched->queue		= new_pqueue;
			new_sched->is_running	= FALSE;
		}
		else /* case PQCreate failed */
		{
			free(new_sched);
			new_sched = NULL;
		}
	}
	
	return (new_sched);
}


/******************************************************************************
*								SchedulerDestroy
*******************************************************************************/
void SchedulerDestroy(scheduler_t *scheduler)
{
	assert(scheduler);
	
	/* popout all of the tasks + destroys them */
	while (!SchedulerIsEmpty(scheduler))
	{
		TaskDestroy(PQPop(scheduler->queue));
	}
	
	/* destroys the queue */
	PQDestroy(scheduler->queue);
	scheduler->queue = NULL;
	
	/* destroys the scheduler */
	free(scheduler);
	scheduler = NULL;
}


/******************************************************************************
*								SchedulerAddTask
*******************************************************************************/
unique_id_t SchedulerAddTask(scheduler_t *scheduler, task_func_t task_func,
							 void * data, time_t start_time, time_t interval)
{
	task_t *new_task = NULL;
	
	assert(scheduler);
	assert(task_func);
	
	/* creates a new_task */
	new_task = TaskCreate(task_func, data, start_time, interval);
	if (NULL != new_task)
	{
		/* adds new_task to the queue */
		if (SUCCESS == PQPush(scheduler->queue, new_task, NULL))
		{
			return (TaskGetId(new_task));
		}
		
		TaskDestroy(new_task);
		new_task = NULL;
	}

	return (UIDCreateBad());
}


/******************************************************************************
*								SchedulerRemoveTask
*******************************************************************************/
int SchedulerRemoveTask(scheduler_t *scheduler, unique_id_t id)
{
	task_t *task_to_delete = NULL;
	int ret_status = SUCCESS;
	
	assert(scheduler);
	
	/* searches for a matching ID + erases the matched task from list */
	task_to_delete = PQErase(scheduler->queue, IDIsMatch, &id);
	if (NULL != task_to_delete)
	{
		TaskDestroy(task_to_delete);
		task_to_delete = NULL;
	}
	else
	{
		ret_status = FAILURE;
	}
	
	return (ret_status);
}


/******************************************************************************
*								SchedulerRun
*******************************************************************************/
int SchedulerRun(scheduler_t *scheduler)
{
	task_t *task_to_execute = NULL;
	int task_run_status = 0;
	time_t task_run_time = 0;
	time_t time_to_sleep = 0;
	
	assert(scheduler);
	
	scheduler->is_running = TRUE;
	
	while (!SchedulerIsEmpty(scheduler) &&
			TRUE == scheduler->is_running)
	{
		task_to_execute = PQPeek(scheduler->queue);
		task_run_time = TaskGetRunTime(task_to_execute);
		
		/* case the time hasn't come to execute the next mission - sleeps
		   until then */
		if (task_run_time > time(NULL))
		{
			/* makes sure it sleeps enough time */
			time_to_sleep = task_run_time - time(NULL);
			while (time_to_sleep > 0)
			{
				time_to_sleep = sleep(time_to_sleep);
			}
		}
		
		task_to_execute = PQPop(scheduler->queue);
		
		task_run_status = TaskRun(task_to_execute);
		switch (task_run_status)
		{
			case FAIL:
				fprintf(stderr, "ERROR: the task has failed.\n");
				TaskDestroy(task_to_execute);
				break;
				
			case DONE:
				TaskDestroy(task_to_execute);
				task_to_execute = NULL;
				break;
				
			case REPEAT:
				TaskUpdateRunTime(task_to_execute);
				/* push it back & checks */
				if (SUCCESS != PQPush(scheduler->queue, task_to_execute, NULL))
				{
					fprintf(stderr, "ERROR: cannot repeat this task.\n");
					TaskDestroy(task_to_execute);
					task_to_execute = NULL;
				}
				break;
				
			default:
				break;
		}
	}
	
	return ((TRUE == scheduler->is_running) ? COMPLETE : STOP);
}


/******************************************************************************
*								SchedulerStop
*******************************************************************************/
void SchedulerStop(scheduler_t *scheduler)
{
	assert(scheduler);
	
	scheduler->is_running = FALSE;
}


/******************************************************************************
*								SchedulerSize
*******************************************************************************/
size_t SchedulerSize(scheduler_t *scheduler)
{
	assert(scheduler);
	
	return (PQSize(scheduler->queue));
}


/******************************************************************************
*								SchedulerIsEmpty
*******************************************************************************/
int SchedulerIsEmpty(scheduler_t *scheduler)
{
	assert(scheduler);
	
	return (PQIsEmpty(scheduler->queue));
}



/******************************************************************************
*************************** internal functions ********************************
*******************************************************************************/

/*************************** HasHigherPriority *******************************/
static int HasHigherPriority(void *queue_tasks, const void *new_task, void *param)
{
	time_t run_time_queue_task = 0;
	time_t run_time_new_task = 0;
	UNUSED(param);
	
	assert(queue_tasks);
	assert(new_task);
	
	/* extract the run_time from the tasks */
	run_time_queue_task = TaskGetRunTime((task_t *) queue_tasks);
	run_time_new_task = TaskGetRunTime((task_t *) new_task);
	
	return (run_time_queue_task > run_time_new_task);
}

/****************************** IDIsMatch *************************************/
static int IDIsMatch(void *task_in_queue, void *ptr_id_to_check)
{
	unique_id_t id_task_in_queue = {0};
	unique_id_t	id_to_check		 = {0};
	
	assert(task_in_queue);
	assert(ptr_id_to_check);
	
	/* put in local variables in the right type */
	id_task_in_queue = TaskGetId((task_t *) task_in_queue);
	id_to_check		 = *(unique_id_t *) ptr_id_to_check;
	
	return (UIDIsSame(id_task_in_queue, id_to_check));
}















