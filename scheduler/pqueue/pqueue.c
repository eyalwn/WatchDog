/*******************************************************************************
*	Filename :		pqueue.c
*	Developer :		Eyal Weizman
*	Last Update :	2019-04-04
*	Description:	pqueue source file
*******************************************************************************/

#include <stdlib.h>	/* free */
#include <assert.h> /* assert */

#include "./heap/heap.h"
#include "pqueue.h"

/******************************* MACROS ***************************************/
#define HEAP_CAPACITY 10

/*** enums ***/
/*enum status*/
/*{*/
/*	SUCCESS = 0,*/
/*	FAILURE = 1*/
/*};*/

/*** structures ***/
struct pqueue
{
	heap_t *heap;
};


/******************************************************************************
*								PQCreate
*******************************************************************************/
pqueue_t *PQCreate(has_higher_priority_t prior_func)
{
	pqueue_t *pq = NULL;
	heap_t *heap = NULL;
	
	assert(prior_func);
	
	pq = (pqueue_t *) malloc(sizeof(pqueue_t));
	if (NULL == pq)
	{
		return (NULL);
	}
	
	heap = HeapCreate(HEAP_CAPACITY ,prior_func, NULL);
	if (NULL == heap)
	{
		free(pq);
		pq = NULL;
		return (NULL);
	}
	
	pq->heap = heap;
	return (pq);
}


/******************************************************************************
*								PQDestroy
*******************************************************************************/
void PQDestroy(pqueue_t *pqueue)
{
	assert(pqueue);
	
	HeapDestroy(pqueue->heap);
	pqueue->heap = NULL;
	free(pqueue);
	pqueue = NULL;
}


/******************************************************************************
*								PQPush
*******************************************************************************/
int PQPush(pqueue_t *pqueue, void *data, void *param)
{
	status_t status = SUCCESS;
	
	assert(pqueue);
	
	HeapSetParam(pqueue->heap, param);
	status = HeapPush(pqueue->heap, data);
	
	return (status);
}


/******************************************************************************
*								PQPop
*******************************************************************************/
void *PQPop(pqueue_t *pqueue)
{
	void *popped_data = NULL;
	
	assert(pqueue);
	
	popped_data = HeapPeek(pqueue->heap);
	HeapPop(pqueue->heap);
	
	return (popped_data);
}


/******************************************************************************
*								PQIsEmpty
*******************************************************************************/
int PQIsEmpty(const pqueue_t *pqueue)
{
	assert(pqueue);
	
	return (HeapIsEmpty(pqueue->heap));
}


/******************************************************************************
*								PQPeek
*******************************************************************************/
void *PQPeek(const pqueue_t *pqueue)
{
	assert(pqueue);
	
	return (HeapPeek(pqueue->heap));
}


/******************************************************************************
*								PQSize
*******************************************************************************/
size_t PQSize(const pqueue_t *pqueue)
{
	assert(pqueue);
	
	return (HeapSize(pqueue->heap));
}


/******************************************************************************
*								PQClear
*******************************************************************************/
void PQClear(pqueue_t *pqueue)
{
	assert(pqueue);
	
	while (!PQIsEmpty(pqueue))
	{
		PQPop(pqueue);
	}
}


/******************************************************************************
*								PQErase
*******************************************************************************/
void *PQErase(pqueue_t *pqueue, pq_is_match_t func, void *param)
{
	assert(pqueue->heap);
	
	return (HeapRemove(pqueue->heap, param, func));
}











