/*******************************************************************************
*	Filename	:	wd_shared.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-05-18
*	Description	:	watchdog shared object source file. has functions common
					to both wd_outer and wd_api.c.
*******************************************************************************/
#define _POSIX_C_SOURCE 200112L   /* pthread_sigmask, setenv */

#include <assert.h> /* assert */
#include <time.h>	/* time */

#ifndef NDEBUG
#include <stdio.h> 	/* printf */
#endif

#include "./scheduler/scheduler.h"
#include "wd_shared.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)
#define SEND_INTERVAL (1)
#define CHECK_INTERVAL (2)
#define MAX_SECONDS_WAITING (3)
#define COUNT_1_SEC_INTERVAL (1)

/************************* global variable ************************************/
/* NOTE: these global static variables are created separatly for each process */

/* 	counts how many seconds have passed since the last time SIGUSR1 was 
	received */
static int g_time_since_last_sig = 0;

/*  controls whether to stay in the main loop */
static int g_keep_run = TRUE;

/* a pointer to the scheduler */
static scheduler_t *g_sched = NULL;

/******************************************************************************
*						shared functions
*******************************************************************************/
/************************* InitScheduler **************************************/
status_t InitScheduler(com_pack_t *com_pack)
{
	status_t ret_status = FAILURE;
	
	assert(com_pack);
	
	/* create scheduler + check whether worked */
	g_sched = SchedulerCreate();
	if (NULL != g_sched)
	{
		/* laoding scheduler with tasks */
		com_pack->task1_uid = SchedulerAddTask(g_sched,
									  SendSignalTask,
									  &(com_pack->other_proc_pid),
									  time(NULL),
									  SEND_INTERVAL);
									  
		com_pack->task2_uid = SchedulerAddTask(g_sched,
									  CheckSignalTask,
									  g_sched,
									  time(NULL),
									  CHECK_INTERVAL);
									  
		com_pack->task3_uid = SchedulerAddTask(g_sched,
									  CounterAddOneTask,
									  NULL,
									  time(NULL),
									  COUNT_1_SEC_INTERVAL);
		ret_status = SUCCESS;
	}
	
	return (ret_status);
}


/************************** MainLoop ******************************************/
void MainLoop(com_pack_t *com_pack)
{
	assert(com_pack);
	
	/*** main loop - keeps run scheduler and revive as long as the 
		g_keep_run flag has TRUE ***/
	while (g_keep_run)
	{
		/* run sched */
		SchedulerRun(g_sched);
	
		/* if scheduler stopped - Revive other proc */
		Revive(com_pack);
	}
}


/************************** Revive ********************************************/
void Revive(com_pack_t *com_pack)
{
	pid_t *other_proc_pid = NULL;
	
	assert(com_pack);
	
	if (g_keep_run)
	{
		/* set local vairables */
		other_proc_pid = &(com_pack->other_proc_pid);
		
		#ifndef NDEBUG
		printf("\n ***Revive!!! ***\n\n");
		#endif
		
		/* revive the watched process */
		*other_proc_pid = fork();
		if (*other_proc_pid == 0)
		/* child */
		{
			/* revive the other process */
			execv(com_pack->who_to_revive, com_pack->argv);
		}
		else if (*other_proc_pid > 0)
		{
			/* waits untill revived proc is ready */
			WaitForSignal(com_pack);			
		}
		/* if fork failed - will try again to revive anyway */		
	}
}


/************************** WaitForSignal *************************************/
void WaitForSignal(com_pack_t *com_pack)
{
	assert(com_pack);
	
	/*  waits untill a signal is received & prevents from additional signals
		to interrupt */
	pthread_sigmask(SIG_BLOCK, &(com_pack->mask), &(com_pack->oldmask));
	
	#ifndef NDEBUG
	printf("\n%d waits for signal\n", getpid());
	#endif
	
	sigsuspend(&(com_pack->oldmask));
	pthread_sigmask(SIG_UNBLOCK, &(com_pack->mask), NULL);
}

/************************** SendSignalTask ************************************/
int SendSignalTask(void *arg)
{
	assert(arg);
	
	#ifndef NDEBUG
	printf("%d send signal to %d\n", getpid(), *(pid_t *)arg);
	#endif
	
	kill(*(pid_t *)arg, SIGUSR1);
	
	return (REPEAT);
}


/************************* CounterAddOneTask **********************************/
int CounterAddOneTask(void *arg)
{
	UNUSED(arg);
	
	/* mark the current call to this function */
	++g_time_since_last_sig;
	
	#ifndef NDEBUG
	printf("%d counter is now: %d  \n", getpid(), g_time_since_last_sig);
	#endif
	
	return (REPEAT);
}


/************************** CheckSignalTask ***********************************/
/* this func is of type task_func_t */
int CheckSignalTask(void *arg)
{
	scheduler_t *sched = (scheduler_t *)arg;
	
	assert(arg);
	
	/* check if too much time has past since the last SIGUSR1 was received */
	if (g_time_since_last_sig > MAX_SECONDS_WAITING)
	{
		SchedulerStop(sched);
		
		/* zero to counter before revive the process */
		g_time_since_last_sig = 0;
	}
	
	return (REPEAT);		
}


/************************** SigHandZeroCounter ***************************************/
void SigHandZeroCounter(int signal)
{
	UNUSED(signal);
	
	#ifndef NDEBUG
	printf("%d received the signal\n", getpid());
	#endif
	
	/* zero the counter */
	g_time_since_last_sig = 0;
	
	return;
}


/************************** SigHandStopMainLoop ***************************************/
void SigHandStopMainLoop(int signal)
{
	UNUSED(signal);
	
	/* stops main loop */
	StopMainLoop();
}


/************************ StopMainLoop ****************************************/
void StopMainLoop(void)
{
	g_keep_run = FALSE;
	SchedulerStop(g_sched);
}


/******************* SchedulerDestroyWrapper **********************************/
void SchedulerDestroyWrapper(void)
{
	SchedulerDestroy(g_sched);
}
