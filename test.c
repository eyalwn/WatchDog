/******************************************************************************
*	Filename	:	test.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2020-03-08
*	Description	:	watchdog test file
*******************************************************************************/
#include <stdio.h> 		/* printf */
#include <unistd.h>     /* sleep */

#include "wd_api.h"

/******************************************************************************
*								main
*******************************************************************************/
int main(int argc, char *const argv[])
{
	status_t ret_val = 0;
	
	printf("\n***** TEST FOR WATCHDOG'S FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");
	
	printf("APP pid is: %d\n", getpid());
	
	ret_val = WDKeepMeAlive(argc, argv);
	
	printf("\nret status from Keep: %d\n\n", ret_val);
	
	sleep(10);
	
	printf("\nAPP: let me die!!:\n");
	WDLetMeDie();
	
	sleep(2);
	printf("\n*****END OF APP****\n");
	printf("\n\n--------------------------------------------------------\n\n");

	return (0);
}


