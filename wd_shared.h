/******************************************************************************
 * File name  : watchdog_shared.h
 * Developer  : Eyal Weizman
 * Date		  : 2019-05-18
 * Description: header for the functions of watchdog_shared.c
 ******************************************************************************/
#ifndef _WATCHDOG_SHARED_H_
#define _WATCHDOG_SHARED_H_

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L    /*   defined here so the header will know 
										struct sigaction */
#endif

#include <unistd.h>     /* getpid, getppid, fork, execv  */
#include <signal.h>		/* struct sigaction, sigaction, sigemptyset, sigaddset*/

#include "./scheduler/task/uid/uid.h"
#include "./utils/general_types.h"

typedef struct sigaction action_t;

/*** structures ****/
/*  this struct contains all the needed variables for the communication thread
	and for the WD. */
typedef struct com_pack_s
{
	char *const 	*argv;
	char 			*who_to_revive;
	pid_t 			other_proc_pid;
	action_t 		sig_1;
	action_t 		sig_2;
	sigset_t 		mask;
	sigset_t		oldmask;
	unique_id_t 	task1_uid;
	unique_id_t 	task2_uid;
	unique_id_t 	task3_uid;
}com_pack_t;


/*** MACROS for both watchdog.c and watchdog_main.c ***/
#define SEND_INTERVAL (1)
#define CHECK_INTERVAL (2)
#define MAX_SECONDS_WAITING (3)

/* main routine functions */
void MainLoop(com_pack_t *com_pack);
status_t InitScheduler(com_pack_t *com_pack);
void Revive(com_pack_t *com_pack);
void WaitForSignal(com_pack_t *com_pack);

/* tasks functions for the scheduler */
int SendSignalTask(void *arg);

/* increase the global counter by 1 */
int CounterAddOneTask(void *arg);

/* checks if the max time limit for receiving a signal has expired */
int CheckSignalTask(void *arg);

/* signal handlers for SIGUSR1/SIGUSR2 respectively */
void SigHandZeroCounter(int signal);
void SigHandStopMainLoop(int signal);

/* ending functions */
void StopMainLoop(void);
void SchedulerDestroyWrapper(void);

#endif /* _WATCHDOG_SHARED_H_ */
