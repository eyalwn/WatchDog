/*******************************************************************************
*	Filename	:	wd_api.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-05-18
*	Description	:	watchdog source file
*******************************************************************************/	
#define _XOPEN_SOURCE               /* sigset_t, SIGBLOCK */
#define _POSIX_C_SOURCE 200112L   /* pthread_sigmask, setenv */

#include <stdlib.h>		/* setenv */
#include <assert.h> 	/* assert */
#include <pthread.h>	/* pthread_create, pthread_join */

#ifndef NDEBUG
#include <stdio.h> 		/* printf */
#endif

#include "./scheduler/scheduler.h"
#include "wd_api.h"
#include "wd_shared.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)
#define SECONDS_TO_WAIT_FOR_SIGNAL (5)

/************************* internal functions *********************************/
/*  creates the WD proc, informs 'WDKeepMeAlive' func whether it succeeded,
	and operates the communication between this proc & the WD. */
static void *ComThread(void *argv);

/*  prepare the communication thread for the start of the communication
	routine with WD. */
static void InitCom(com_pack_t *com_pack);

/************************* global variable ************************************/
/*  holds the value of the returned status from WDKeepMeAlive */
static status_t g_keep_me_alive_status = FAILURE;

/******************************************************************************
*							WDKeepMeAlive
*******************************************************************************/
status_t WDKeepMeAlive(int argc, char *const argv[])
{
	pthread_t com_thread_id = 0;
	sigset_t mask;
	sigset_t oldmask;
	/* set the maximal time to wait for a signal from WD */
	struct timespec timeout = {SECONDS_TO_WAIT_FOR_SIGNAL, 0};
	
	UNUSED(argc);
	
	/* create a detatched communication_thread + checks */
	if (0 == pthread_create(&com_thread_id, NULL, ComThread, (char **)argv))
	{
		/* make ComThread independent */
		pthread_detach(com_thread_id);
		
		/*  creating a mask with SIGUSR1 */
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		
		/* block SIGUSR1 from to the whole app except ComThread */
		pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
		
		/* wait till SIGUSR1 is received or till timeout is expired */
		sigtimedwait(&mask, NULL, &timeout);
	}
	
	return (g_keep_me_alive_status);
}


/************************** ComThread ***************************************/
static void *ComThread(void *argv)
/*  creates the WD proc, informs 'WDKeepMeAlive' func whether it succeeded,
	and operates the communication between this proc & the WD. */
{
	/*	create a pack of arguments via stack allocation */
	com_pack_t com_pack = {0};
	
	assert(argv);
	
	/* save argv in com_pack */
	com_pack.argv = (char *const *)argv;
	
	/* init the communication to WD */
	InitCom(&com_pack);
	
	/* check if initiated successfully */
	if (SUCCESS == g_keep_me_alive_status)
	{
		/* communicates with WD */
		MainLoop(&com_pack);
		
		/* stop WD by sending signal SIGUSR2 */
		kill(com_pack.other_proc_pid, SIGUSR2);
		
		/* free resources */
		SchedulerDestroyWrapper();
	}
	
	#ifndef NDEBUG
	printf("\n***ComThread is dead***\n\n");
	#endif
	
	return (NULL);
}


/************************** InitCom ***************************************/
/*  prepare the communication thread for the start of the communication
	routine with WD. */
static void InitCom(com_pack_t *com_pack)
{
	pid_t *wd_pid = NULL;
	
	assert(com_pack);
	
	/* set local vairable */
	wd_pid = &(com_pack->other_proc_pid);
	
	/*  define the behaviour when SIGUSR1 is received (call SigHandZeroCounter when
		received) */
	com_pack->sig_1.sa_handler = SigHandZeroCounter;
	sigaction(SIGUSR1, &(com_pack->sig_1), NULL);
	
	/* set mask (used later by sigsuspend) */
	sigemptyset(&(com_pack->mask));
	sigaddset(&(com_pack->mask), SIGUSR1);
	
	/* saves the path to WD */
	com_pack->who_to_revive = "watchdog.out";
	
	/* create a scheduler & load it with tasks + check */
	if (SUCCESS == InitScheduler(com_pack))
	{
		g_keep_me_alive_status = SUCCESS;
									  
		/* checks whether WD is allready alive */
		if (NULL == getenv("WD_ALIVE"))
		/* wd is not alive yet */
		{
			/*	marking that now there is a watchdog alive by setting a new
				enviroment variable.
				in case fork will fail - the env will be deleted */
			setenv("WD_ALIVE", "1", 0);
			
			#ifndef NDEBUG
			printf("APP: create WD\n");
			#endif
			
			/*  create watchdog & saves the new pid into 
				com_pack->other_proc_pid via the pointer. */
			*wd_pid = fork();
			if (*wd_pid == 0)
			/* child */
			{
				execv(com_pack->who_to_revive, com_pack->argv);
			}
			else if (*wd_pid < 0)
			/* if fork failed */
			{
				unsetenv("WD_ALIVE");
				g_keep_me_alive_status = FAILURE;
				return;
			}
			
			/* waits untill WD sends a signal */
			WaitForSignal(com_pack);
		}
		else
		/* WD allready alive */
		{
			/* WD is the parent right now */
			*wd_pid = getppid();
			
			#ifndef NDEBUG
			printf("WD is allready alive. pid: %d\n", com_pack->other_proc_pid);
			#endif
		}
	}
}


/******************************************************************************
*							WDLetMeDie
*******************************************************************************/
void WDLetMeDie(void)
{
	/* stop the main loop */
	StopMainLoop();
}











