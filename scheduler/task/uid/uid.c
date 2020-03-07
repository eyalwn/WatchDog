/*******************************************************************************
*	Filename :		uid.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-04-30
*	Description:	a MT-safe uid generator source file
*******************************************************************************/
#include <stdlib.h>		/* malloc, free */
#include <assert.h>		/* assert */
#include <time.h>		/*  */
#include <unistd.h>		/* getpid */
#include <pthread.h>	/* pthread_mutex_lock, pthread_mutex_unlock */

#include "uid.h"

/******************************************************************************
*								UIDCreate
*******************************************************************************/
unique_id_t UIDCreate(void)
{
	unique_id_t uid = {0};
	static size_t counter = 0;
	
	uid.time = time(NULL);
	uid.pid = getpid();
	
	__sync_add_and_fetch(&uid.counter, __sync_fetch_and_add(&counter, 1));

	return (uid);
}


/******************************************************************************
*								UIDIsSame
*******************************************************************************/
int UIDIsSame(unique_id_t uid1, unique_id_t uid2)
{
	return ((uid1.time		)	==	(uid2.time		)	&&
			(uid1.pid		)	==	(uid2.pid		)	&&
			(uid1.counter	)	==	(uid2.counter	));
}


/******************************************************************************
*								UIDIsBad
*******************************************************************************/
int UIDIsBad(unique_id_t uid)
{
	return ((0	==	uid.time	)	&&
			(0	==	uid.pid		)	&&
			(0	==	uid.counter)	);
	
}


/******************************************************************************
*								UIDCreateBad
*******************************************************************************/
unique_id_t UIDCreateBad(void)
{
	unique_id_t bad_uid = {0};
	
	return (bad_uid);
}





