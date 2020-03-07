/******************************************************************************
*	Filename	:	wd_outer.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-05-18
*	Description	:	source file for the outer WD process.
*******************************************************************************/
#define _POSIX_C_SOURCE 200112L    /* struct sigaction */

#include <assert.h> 	/* assert */

#ifndef NDEBUG
#include <stdio.h> 		/* printf */
#endif

#include "./scheduler/scheduler.h"
#include "wd_shared.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)

/************************** internal functions ********************************/
static void InitWD(com_pack_t *wd_pack);

/******************************************************************************
*								main
*******************************************************************************/
int main(int argc, char *const argv[])
{
	/*	create a pack of arguments via stack allocation */
	com_pack_t com_pack = {0};
	
	UNUSED(argc);
	
	#ifndef NDEBUG
	printf("WD pid is: %d\n", getpid());
	printf("\n *** WD: IM ALIVE!!!***\n\n");
	#endif
	
	com_pack.argv = argv;
	InitWD(&com_pack);
	
	/* main loop - can be stopped by SIGUSR2 */
	MainLoop(&com_pack);
	
	/* free resources */
	SchedulerDestroyWrapper();
	
	#ifndef NDEBUG
	printf("\n ***WD is dead ***\n\n");
	#endif
	
	return (0);
}


/************************** InitWD ********************************************/
static void InitWD(com_pack_t *com_pack)
{
	assert(com_pack);
	
	/* at this point, the parent process is necessarily the app */
	com_pack->other_proc_pid = getppid();
	
	/* define the behaviour when SIGUSR1/SIGUSR2 are received */
	com_pack->sig_1.sa_handler = SigHandZeroCounter;
	sigaction(SIGUSR1, &(com_pack->sig_1), NULL);
	com_pack->sig_2.sa_handler = SigHandStopMainLoop;
	sigaction(SIGUSR2, &(com_pack->sig_2), NULL);
	
	/* set mask */
	sigemptyset(&(com_pack->mask));
	sigaddset(&(com_pack->mask), SIGUSR1);
	
	/* set the app name (from argv[0]) in who_to_revive */
	com_pack->who_to_revive = com_pack->argv[0];
	
	/* create scheduler & load it with tasks */
	InitScheduler(com_pack);
}







