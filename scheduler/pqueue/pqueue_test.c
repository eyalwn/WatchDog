/*******************************************************************************
*	Filename :		pqueue_test.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-02-18
*	Description :	pqueue test file
*******************************************************************************/
#include <stdio.h> 		/* printf */

#include "pqueue.h"


/*** MACROS ***/
#define UNUSED(x) ((void) x)

/*** enums ***/
enum boolean_ret_val
{
	FALSE,
	TRUE
};

/*** unit-test functions ***/
void PQCreateTest(void);
void PQPeekTest(void);
void PQSizeTest(void);
void PQPushTest(void);
void PQPopTest(void);
void PQIsEmptyTest(void);
void PQClearTest(void);
void PQEraseTest(void);

/*** internal functions ***/
int SortIntByValue(void *list_data, const void *new_data, void *param);
int IntIsEqual(void *data, void *param);


/*****************************************************************************
*								main
******************************************************************************/
int main(void)
{
	printf("\n***** TEST RUN FOR PRIORITY QUEUE FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");

	PQCreateTest();
	printf("\n\n--------------------------------------------------------\n\n");

	printf("PQDestroy:\t\t\t(RUN WITH VALGRIND)");
	printf("\n\n--------------------------------------------------------\n\n");

	PQPeekTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQSizeTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQPushTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQPopTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQIsEmptyTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQClearTest();
	printf("\n\n--------------------------------------------------------\n\n");

	PQEraseTest();
	printf("\n\n--------------------------------------------------------\n\n");

	return (0);
}


/******************************************************************************
*								Unit-Tests
*******************************************************************************/

/************************* PQCreateTest ***************************************/
void PQCreateTest(void)
{
	pqueue_t *pq = NULL;

	printf("PQCreate:\t\t\t");

	pq = PQCreate(SortIntByValue);
	(NULL != pq)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQPeekTest *****************************************/
void PQPeekTest(void)
{
	pqueue_t *pq = NULL;
	void *ret_val_1 = NULL;
	void *ret_val_2 = NULL;
	int a = 15;
	int b = 7;


	printf("PQPeek:\t\t\t\t");

	pq = PQCreate(SortIntByValue);
	ret_val_1 = PQPeek(pq);

	PQPush(pq, &a, NULL);
	PQPush(pq, &b, NULL);
	ret_val_2 = PQPeek(pq);

	(NULL == ret_val_1) &&
	(&b == ret_val_2)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQSizeTest *****************************************/
void PQSizeTest(void)
{
	pqueue_t *pq = NULL;
	size_t ret_val_1 = 0;
	size_t ret_val_2 = 0;
	int a = 15;
	int b = 7;

	printf("PQSize:\t\t\t\t");

	pq = PQCreate(SortIntByValue);
	ret_val_1 = PQSize(pq);

	PQPush(pq, &a, NULL);
	PQPush(pq, &b, NULL);
	ret_val_2 = PQSize(pq);

	(0 == ret_val_1) &&
	(2 == ret_val_2)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQPushTest *****************************************/
void PQPushTest(void)
{
	pqueue_t *pq = NULL;
	int ret_val_1 = 0;
	int ret_val_2 = 0;
	size_t ret_val_3 = 0;
	int a = 15;
	int b = 7;
	int c = 20;
	int d = 100;

	printf("PQPush:\t\t\t\t");

	pq = PQCreate(SortIntByValue);
	ret_val_1 = PQPush(pq, &a, NULL);	/* expected value: 0 */

	PQPush(pq, &b, NULL);
	PQPush(pq, &c, NULL);
	PQPush(pq, &d, NULL);

	ret_val_2 = *(int *)PQPeek(pq);	/* expected value: 7 */
	ret_val_3 = PQSize(pq);			/* expected value: 4 */

	(0 == ret_val_1) &&
	(7 == ret_val_2) &&
	(4 == ret_val_3)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQPopTest ******************************************/
void PQPopTest(void)
{
	pqueue_t *pq = NULL;
	void *ret_val_1 = NULL;
	int ret_val_2 = 0;
	size_t ret_val_3 = 0;
	int a = 15;
	int b = 7;
	int c = 20;

	printf("PQPop:\t\t\t\t");

	pq = PQCreate(SortIntByValue);
	ret_val_1 = PQPop(pq);			/* expected value: NULL */

	PQPush(pq, &a, NULL);
	PQPush(pq, &b, NULL);
	PQPush(pq, &c, NULL);

	ret_val_2 = *(int *)PQPop(pq);	/* expected value: b=7 */

	PQPop(pq);
	ret_val_3 = PQSize(pq);			/* expected value: 1 */

	(NULL == ret_val_1) &&
	(7 == ret_val_2) 	&&
	(1 == ret_val_3)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQIsEmptyTest **************************************/
void PQIsEmptyTest(void)
{
	pqueue_t *pq = NULL;
	int ret_val_1 = 0;
	int ret_val_2 = 0;
	int a = 15;

	printf("PQIsEmpty:\t\t\t");

	pq = PQCreate(SortIntByValue);
	ret_val_1 = PQIsEmpty(pq);		/* expected value: 1 */

	PQPush(pq, &a, NULL);
	ret_val_2 = PQIsEmpty(pq);		/* expected value: 0 */

	(1 == ret_val_1) &&
	(0 == ret_val_2)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQClearTest ****************************************/
void PQClearTest(void)
{
	pqueue_t *pq = NULL;
	int ret_val_1 = 0;
	int a = 15;
	int b = 7;
	int c = 20;

	printf("PQClear:\t\t\t");

	pq = PQCreate(SortIntByValue);
	PQPush(pq, &a, NULL);
	PQPush(pq, &b, NULL);
	PQPush(pq, &c, NULL);

	PQClear(pq);
	ret_val_1 = PQIsEmpty(pq);		/* expected value: 1 */

	(1 == ret_val_1)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/************************* PQEraseTest ****************************************/
void PQEraseTest(void)
{
	pqueue_t *pq = NULL;
	int ret_val_1 = 0;
	size_t count_after_erase = 0;
	int a = 15;
	int b = 7;
	int c = 20;

	printf("PQErase:\t\t\t");

	pq = PQCreate(SortIntByValue);
	PQPush(pq, &a, NULL);
	PQPush(pq, &b, NULL);
	PQPush(pq, &c, NULL);
	
	ret_val_1 = *(int *)PQErase(pq, IntIsEqual, &a);	/* expected value: 15 */
	count_after_erase = PQSize(pq);						/* expected: 2 */
		
	(15 == ret_val_1)	&&
	(2 == count_after_erase)
	?
	printf("SUCCESS") : printf("FAIL");

	PQDestroy(pq);
}


/******************************************************************************
*						internal functions
*******************************************************************************/

/*************************** SortIntByValue ***********************************/
int SortIntByValue(void *list_data, const void *new_data, void *param)
{
	/* should sort from the smallest to the biggest values of integers */
	UNUSED(param);

	return (*(int *) new_data < *(int *) list_data);
}


/****************************** IntIsEqual*************************************/
int IntIsEqual(void *data, void *param)
{
	return ((*(int *) data == *(int *) param) ? TRUE : FALSE);
}
