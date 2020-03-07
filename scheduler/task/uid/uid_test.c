/*******************************************************************************
*	Filename :		uid_test.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-04-30
*	Description :	MT-safe uid generator test file
*******************************************************************************/
#include <stdio.h> 		/* printf */
#include <pthread.h>	/* create, join */

#include "uid.h"

/********************** MACROS ************************************************/
/* with this values - the probability of catching duplicates is very high */
#define NUM_OF_UIDS		(10000)
#define NUM_OF_THREADS	(2)
#define DIV ((NUM_OF_UIDS) / (NUM_OF_THREADS))

/****************** Test Functions ********************************************/
void SingleThreadTest(void);
static void MTTest(void);

/****************** internal Functions ****************************************/
static void *UIDCreateWrapper(void *arg);
void PrintID(unique_id_t uid);

/*****************************************************************************
*								main
******************************************************************************/
int main(void)
{
	printf("\n***** INTEGRATED TEST FOR UID'S FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");
	
	SingleThreadTest();
	printf("\n--------------------------------------------------------\n\n");
	
	MTTest();
	printf("\n--------------------------------------------------------\n\n");
	
	return (0);
}


/******************************************************************************
*								Unit-Tests
*******************************************************************************/

/************************* SingleThreadTest **********************************/
void SingleThreadTest(void)
{
	unique_id_t uid_1 = {0};
	unique_id_t uid_2 = {0};
	
	printf("Single-Thread Test:\n\n");
	
	printf("IsBad: when not initialized:\t\t\t");
	(TRUE == UIDIsBad(uid_1)) ?
	printf("SUCCESS") : printf("FAIL");
	
	uid_1 = UIDCreate();
	uid_2 = UIDCreate();
	
	printf("\nIsSame: are the uids are different?\t\t");
	(FALSE == UIDIsSame(uid_1, uid_2)) ?
	printf("SUCCESS") : printf("FAIL");
}


/************************** MTTest ********************************************/
static void MTTest(void)
{
	pthread_t threads_id_arr[NUM_OF_THREADS] = {0};
	unique_id_t 	 uid_arr[NUM_OF_UIDS] = {{0}};
	int i = 0;
	int j = 0;
	
	printf("Multi-Thread Test:\n\n");
	
	/* send each uid address to a thread that will call UIDCreate */
	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		pthread_create(	&threads_id_arr[i], NULL, UIDCreateWrapper,
						&uid_arr[i * DIV]);
	}
	
	/* join all */
	for (i = 0; i < NUM_OF_THREADS; ++i)
	{
		pthread_join(threads_id_arr[i], NULL);
	}
	
	/* compare every possible pair */
	printf("prevents UID duplication:\t\t\t");
	for (i = 0; i < NUM_OF_UIDS - 1; ++i)
	{
		for (j = i + 1; j < NUM_OF_UIDS; ++j)
		{
			if (TRUE == UIDIsSame(uid_arr[i], uid_arr[j]))
			{
				printf("FAIL - indexes %d, %d are the same\n", i, j);
				
				printf("\n\nthe ID of %d is:\n", i);
				PrintID(uid_arr[i]);
				
				printf("\n\nthe ID of %d is:\n", j);
				PrintID(uid_arr[j]);
				
				return;
			}
		}
	}
	
	printf("SUCCESS\n");	
}

/******************************************************************************
*								local functions
*******************************************************************************/
/************************** UIDCreateWrapper **********************************/
static void *UIDCreateWrapper(void *arg)
{
	int i = 0;
	
	
	
	for (i = 0; i < DIV; ++i)
	{
		((unique_id_t *)arg)[i] = UIDCreate();		
	}
	
	return (NULL);
}


/************************** PrintID *******************************************/
void PrintID(unique_id_t uid)
{
	printf("uid.time:\t%d\n", (int) uid.time);
	printf("uid.pid:\t%d\n", (int) uid.pid);
	printf("uid.count:\t%lu\n", uid.counter);
}



