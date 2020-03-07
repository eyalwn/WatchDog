/******************************************************************************
 * File name  : watchdog.h
 * Developer  : Eyal Weizman
 * Date		  : 2019-05-06
 * Description: watchdog API
 ******************************************************************************/
 
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "./utils/general_types.h"  

/***************************** WDKeepMeAlive **********************************/
/*
 * description  :  Initiates application protection.
 *                 Reruns application incase of unexpected termination.
 *                 Protection is set until WDLetMeDie is called.
 *                 Max wait time on termination is 5 seconds.
 *
 * input		:  main argument argc   - number of program arguments
 *                 main argument argv[] - array of program arguments
 * 
 * return value :  on success - SUCCESS(0).
 * 				   on failure - FAILURE(-1).
 */
status_t WDKeepMeAlive(int argc, char *const argv[]);

/******************************* WDLetMeDie ***********************************/
/*
 * description  :  Terminates WDKeepMeAlive application protection.
 */
void WDLetMeDie(void);

#endif /* _WATCHDOG_H_ */
