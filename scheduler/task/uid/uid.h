/*****************************************************************************
 * File name:   uid.h
 * Developer:   Eyal Weizman
 * Date:        2019-02-21
 * Description: header file for MT-safe UID generator
 *****************************************************************************/

#ifndef __UID_H__
#define __UID_H__

#include <stddef.h>      /* size_t  */
#include <sys/types.h>   /* pid_t   */

#include "../../../utils/general_types.h"

typedef struct unique_id
{
    time_t time;
    pid_t pid;             /* use getpid()       */
    size_t counter;
} unique_id_t;

/*
 * creates a struct and generate unique number (ID) per process.
 * the function return the struct by value (!! - not ptr).
 */
unique_id_t UIDCreate(void);

/*
 * checks if uid1 is equal to uid2.
 * return: 0 - FALSE,
 *         1 - TRUE.
 */
int UIDIsSame(unique_id_t uid1, unique_id_t uid2);

/*
 * checks if uid is valid.
 * return: 0 - FALSE,
 *         1 - TRUE.
 */
int UIDIsBad(unique_id_t uid);

/*
 * Creates a bad uid with all fields equal to 0;
 * Returns the bad uid.
 */
 unique_id_t UIDCreateBad(void);


#endif     /* __UID_H__ */
