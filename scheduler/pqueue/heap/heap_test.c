/******************************************************************************
*	Filename	:	heap_test.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-04-04
*	Description	:	heap test file
*******************************************************************************/
#include <stdio.h> 		/* printf */

#include "heap.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)

/************************** unit-test functions *******************************/
void HeapCreateDestroyTest(void);
void HeapPushTest(void);
void HeapPeekTest(void);
void HeapIsEmptyTest(void);
void HeapSizeTest(void);
void HeapPopTest(void);
void HeapRemoveTest(void);

/************************** internal functions ********************************/
static int IntIsBefore(void *heap_data, const void *new_data,
								   void *param);
static int IntIsMatch(void *new_data, void *heap_data);

/******************************************************************************
*								main
*******************************************************************************/
int main(void)
{
	printf("\n***** UNIT-TEST FOR HEAP'S FUNCTIONS *****\n\n");
	printf("\n========================================================\n\n");
	
	HeapCreateDestroyTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapPushTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapPeekTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapIsEmptyTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapSizeTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapPopTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	HeapRemoveTest();
	printf("\n\n--------------------------------------------------------\n\n");
	
	return (0);
}


/******************************************************************************
*								Unit-Tests
*******************************************************************************/

/************************* HeapCreateDestroyTest ******************************/
void HeapCreateDestroyTest(void)
{
	heap_t *heap = NULL;
	
	printf("Create + Destroy:\t\t\t");
	
	heap = HeapCreate(20, IntIsBefore, NULL);
	
	(NULL != heap)
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}


/************************* HeapPushTest ***************************************/
void HeapPushTest(void)
{
	heap_t *heap = NULL;
	int arr[7] = {1, 2, 3, 4, 5, 6, 7};
	
	printf("Push:\t\t\t\t\t");
	
	heap = HeapCreate(3, IntIsBefore, NULL);
	
	HeapPush(heap, &arr[0]);
	HeapPush(heap, &arr[1]);
	HeapPush(heap, &arr[2]);
	HeapPush(heap, &arr[3]);
	HeapPush(heap, &arr[4]);
	HeapPush(heap, &arr[5]);
	HeapPush(heap, &arr[6]);
	
	printf("SUCCESS");
	
	HeapDestroy(heap);
	heap = NULL;
}


/************************* HeapPeekTest ***************************************/
void HeapPeekTest(void)
{
	heap_t *heap = NULL;
	int *check_val_1 = NULL;
	int *check_val_2 = NULL;
	int arr[7] = {1, 2, 3, 4, 5, 6, 7};
	
	printf("Peek:\t\t\t\t\t");
	
	heap = HeapCreate(3, IntIsBefore, NULL);
	
	check_val_1 = (int *)HeapPeek(heap);	/* expected: null */
	
	HeapPush(heap, &arr[0]);
	HeapPush(heap, &arr[1]);
	HeapPush(heap, &arr[2]);
	HeapPush(heap, &arr[3]);
	HeapPush(heap, &arr[4]);
	HeapPush(heap, &arr[5]);
	HeapPush(heap, &arr[6]);
	
	check_val_2 = (int *)HeapPeek(heap);	/* expected: &arr[6] */
	
	(NULL		== check_val_1)	&&
	(&arr[6]	== check_val_2)
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}


/************************* HeapIsEmptyTest ************************************/
void HeapIsEmptyTest(void)
{
	heap_t *heap = NULL;
	int check_val_1 = 0;
	int check_val_2 = 0;
	int a = 1;
	
	printf("IsEmpty:\t\t\t\t");
	
	heap = HeapCreate(20, IntIsBefore, NULL);
	
	check_val_1 = HeapIsEmpty(heap);
	
	HeapPush(heap, &a);
	check_val_2 = HeapIsEmpty(heap);
	
	(TRUE		== check_val_1)	&&
	(FALSE		== check_val_2)
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}


/************************* HeapSizeTest ***************************************/
void HeapSizeTest(void)
{
	heap_t *heap = NULL;
	size_t check_val_1 = 0;
	size_t check_val_2 = 0;
	int arr[7] = {1, 2, 3, 4, 5, 6, 7};
	
	printf("Size:\t\t\t\t\t");
	
	heap = HeapCreate(3, IntIsBefore, NULL);
	
	check_val_1 = HeapSize(heap);	/* expected: 0 */
	
	HeapPush(heap, &arr[0]);
	HeapPush(heap, &arr[1]);
	HeapPush(heap, &arr[2]);
	HeapPush(heap, &arr[3]);
	HeapPush(heap, &arr[4]);
	HeapPush(heap, &arr[5]);
	HeapPush(heap, &arr[6]);
	
	check_val_2 = HeapSize(heap);	/* expected: 0 */
	
	(0	== check_val_1)	&&
	(7	== check_val_2)
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}

/************************* HeapPopTest ****************************************/
void HeapPopTest(void)
{
	heap_t *heap = NULL;
	status_t check_val_1 = SUCCESS;
	status_t check_val_2 = SUCCESS;
	size_t check_val_3 = 0;
	void  *check_val_4 = NULL;
	size_t check_val_5 = 0;
	void  *check_val_6 = NULL;
	int arr[8] = {100, 80, 40, 60, 70, 30, 20, 50};
	
	printf("Pop:\t\t\t\t\t");
	
	heap = HeapCreate(3, IntIsBefore, NULL);
	
	HeapPush(heap, &arr[0]);
	check_val_1 = HeapPop(heap);	/* expected: SUCCESS */
	check_val_2 = HeapPop(heap);	/* expected: SUCCESS */
	
	HeapPush(heap, &arr[0]);
	HeapPush(heap, &arr[1]);
	HeapPush(heap, &arr[2]);
	HeapPush(heap, &arr[3]);
	HeapPush(heap, &arr[4]);
	HeapPush(heap, &arr[5]);
	HeapPush(heap, &arr[6]);
	HeapPush(heap, &arr[7]);
	
	HeapPop(heap);
	check_val_3 = HeapSize(heap);	/* expected: 7 */
	check_val_4 = HeapPeek(heap);	/* expected: address of 80 (&arr[1])*/
	
	HeapPop(heap);
	check_val_5 = HeapSize(heap);	/* expected: 6 */
	check_val_6 = HeapPeek(heap);	/* expected: address of 70 (&arr[4])*/
	
	(SUCCESS	== check_val_1)	&&
	(SUCCESS	== check_val_2)	&&
	(7			== check_val_3)	&&
	(&arr[1]	== check_val_4)	&&
	(6			== check_val_5)	&&
	(&arr[4]	== check_val_6)	
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}


/************************* HeapRemoveTest *************************************/
void HeapRemoveTest(void)
{
	heap_t *heap = NULL;
	void *check_val_1 = NULL;
	void *check_val_2 = NULL;
	void *check_val_3 = NULL;
	void *check_val_4 = NULL;
	void *check_val_5 = NULL;
	size_t check_val_6 = 0;
	int a = 5;
	int b = 100;
	int c = 70;
	int arr[8] = {100, 80, 40, 60, 70, 30, 20, 50};
	
	printf("Remove:\t\t\t\t\t");
	
	heap = HeapCreate(3, IntIsBefore, NULL);
	
	check_val_1 = HeapRemove(heap, &a, IntIsMatch);	/* expected: NULL */
	
	HeapPush(heap, &arr[0]);
	HeapPush(heap, &arr[1]);
	HeapPush(heap, &arr[2]);
	HeapPush(heap, &arr[3]);
	HeapPush(heap, &arr[4]);
	HeapPush(heap, &arr[5]);
	HeapPush(heap, &arr[6]);
	HeapPush(heap, &arr[7]);
	
	check_val_2 = HeapRemove(heap, &a, IntIsMatch);	/* expected: NULL */
	check_val_3 = HeapRemove(heap, &b, IntIsMatch);	/* expected: &arr[0] */
	check_val_4 = HeapPeek(heap);	/* expected: address of 80 (&arr[1])*/
	HeapRemove(heap, &c, IntIsMatch);
	check_val_5 = HeapPeek(heap);	/* expected: still arr[1] is first*/
	check_val_6 = HeapSize(heap);	/* expected size: 6 */
	
	(NULL		== check_val_1)	&&
	(NULL		== check_val_2)	&&
	(&arr[0]	== check_val_3)	&&
	(&arr[1]	== check_val_4)	&&
	(&arr[1]	== check_val_5)	&&
	(6			== check_val_6)
	?
	printf("SUCCESS") : printf("FAIL");
	
	HeapDestroy(heap);
	heap = NULL;
}


/******************************************************************************
*						internal functions
*******************************************************************************/
/************************* IntIsBefore ****************************************/
static int IntIsBefore(void *heap_data, const void *new_data,
								   void *param)
{
	UNUSED(param);
	
	return (*(int *)new_data >= *(int *)heap_data);
}


/************************* IntIsMatch *****************************************/
static int IntIsMatch(void *new_data, void *heap_data)
{
	return (*(int *)new_data == *(int *)heap_data);
}





