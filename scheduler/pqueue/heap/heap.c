/*******************************************************************************
*	Filename	:	heap.c
*	Developer	:	Eyal Weizman
*	Last Update	:	2019-04-04
*	Description	:	heap source file
*******************************************************************************/
#include <stdlib.h>	/* malloc, free */
#include <assert.h> /* assert */
#include <string.h>	/* memset */

#include "./dynamic_vctor/dynamic_vector.h"
#include "heap.h"

/******************************* MACROS ***************************************/
#define UNUSED(x) ((void) x)
#define SIZE_OF_POINTER (sizeof(void *))

#define FIRST_INDEX 0
#define LEFT_INDEX(x) (x * 2 + 1)
#define RIGHT_INDEX(x) (x * 2 + 2)
#define PARENT_INDEX(x) ((size_t)(((long)(x) - 1) / 2))
#define LAST_INDEX(x) (HeapSize(x) - 1)
#define NO_CHILDS_MARKER 0

/***************************** structures *************************************/
struct heap_s
{
    heap_is_before_t is_before_func;
    void *param;
    dv_t *dv;
};

/************************* internal functions *********************************/
static void HeapifyUpRec(heap_t *heap, size_t current_index);
static void HeapifyDownRec(heap_t *heap, size_t current_index);

/*	finds which element should be before the other. returns its index.
	in case both children are invalid (out of index range) - returns
	NO_CHILDS_MARKER. */
static size_t GetPreferredChild(heap_t *heap, size_t left_index,
											  size_t right_index);

/* 	traverse the heap in search for an element match to 'data'. */
static size_t FindElement(heap_t *heap, void *data,
						  heap_is_match_t is_match_func);

/*	swaps 2 elements indexed with 'index_1', 'index_2'.
	the indexes must be in the valid range!! */
static void SwapElements(heap_t *heap, size_t index_1, size_t index_2);

/* checks if 'index_1' element should be before index_2. */
static boolean_t ElementIsBefore(heap_t *heap, size_t index_1, size_t index_2);

/* debug-tool */
/*static void PrintDv(heap_t *heap);*/

/******************************************************************************
*							HeapCreate
*******************************************************************************/
heap_t *HeapCreate(size_t capacity,
				   heap_is_before_t is_before_func,
                   void *param                      )
{
	heap_t *new_heap = NULL;
	
	assert(is_before_func);
	
	new_heap = (heap_t *)malloc(sizeof(heap_t));
	if (NULL != new_heap)
	{
		new_heap->dv = DVCreate(capacity, SIZE_OF_POINTER);
		if (NULL != new_heap->dv)
		{
			new_heap->is_before_func = is_before_func;
			new_heap->param = param;
		}
		/* case DVCreate failed */
		else
		{
			free(new_heap);
			new_heap = NULL;
		}
	}
	
	return (new_heap);
}

/******************************************************************************
*							HeapDestroy
*******************************************************************************/
void HeapDestroy(heap_t *heap)
{
	assert(heap);
	
	DVDestroy(heap->dv);
	memset(heap, 0, sizeof(heap_t));
	free(heap);
	heap = NULL;
}


/******************************************************************************
*							HeapPush
*******************************************************************************/
status_t HeapPush(heap_t *heap, const void *data)
{
	status_t status = SUCCESS;
	
	assert(heap);
	
	/* saves in vector the ***data pointer*** */
	status = DVPushBack(heap->dv, &data);
	if (SUCCESS == status)
	{
		/* heapify up the new element */
		HeapifyUpRec(heap, LAST_INDEX(heap));
	}
	
	return (status);
}


/************************* HeapifyUpRec ***************************************/
static void HeapifyUpRec(heap_t *heap, size_t current_index)
{
	size_t parent_index = 0;
	
	if (FIRST_INDEX == current_index)
	{
		return;
	}
	
	/* if current should be before parent - make swap + recursive call */
	parent_index = PARENT_INDEX(current_index);
	if (TRUE == ElementIsBefore(heap, current_index, parent_index))
	{
		SwapElements(heap, current_index, parent_index);
		/* recursive call */
		HeapifyUpRec(heap, parent_index);
	}
}


/******************************************************************************
*							HeapPop
*******************************************************************************/
status_t HeapPop(heap_t *heap)
{
	status_t status = SUCCESS;
	
	assert(heap);
	
	if (FALSE == HeapIsEmpty(heap))
	{
		/* swap the 1st and the last elements */
		SwapElements(heap, FIRST_INDEX, LAST_INDEX(heap));
		/* pop last element + get status */
		status = DVPopBack(heap->dv);
		/* heapify down the first_element */
		HeapifyDownRec(heap, FIRST_INDEX);
	}
	
	return (status);
}


/************************* HeapifyDownRec *************************************/
static void HeapifyDownRec(heap_t *heap, size_t current_index)
{
	size_t left_index			= 0;
	size_t right_index			= 0;
	size_t preferred_child_index= 0;
	
	left_index = LEFT_INDEX(current_index);
	right_index = RIGHT_INDEX(current_index);
	
	/* gets the index of the preferred_child */
	preferred_child_index = GetPreferredChild(heap, left_index, right_index);
	
	/* if current has no childs */
	if (NO_CHILDS_MARKER == preferred_child_index)
	{
		return;
	}
	
	/* does preferred_child should be before current? */
	if (TRUE == ElementIsBefore(heap, preferred_child_index, current_index))
	{
		SwapElements(heap, current_index, preferred_child_index);
		/* recursive call */
		HeapifyDownRec(heap, preferred_child_index);
		return;
	}
}


/************************* GetPreferredChild **********************************/
static size_t GetPreferredChild(heap_t *heap, size_t left_index,
											  size_t right_index)
{
	size_t ret_index = NO_CHILDS_MARKER;
	
	assert(heap);
	
	/* when there are 2 childs */
	if (right_index < HeapSize(heap))
	{
		/* configures who should be first? left or right? */
		if (TRUE == ElementIsBefore(heap, left_index, right_index))
		{
			ret_index = left_index;
		}
		else
		{
			ret_index = right_index;
		}
	}
	/* if there is only left_child */
	else if (left_index < HeapSize(heap))
	{
		ret_index = left_index;
	}
	
	return (ret_index);
}


/******************************************************************************
*							HeapRemove
*******************************************************************************/
void *HeapRemove(heap_t *heap, void *data, heap_is_match_t is_match_func)
{
	size_t match_index = 0;
	size_t last_index = 0;
	void *ret_val = NULL;
	
	assert(heap);
	assert(is_match_func);
	
	if (!HeapIsEmpty(heap))
	{
		match_index = FindElement(heap, data, is_match_func);
		last_index = LAST_INDEX(heap);
	
		/* if a match_index was found */
		if (match_index < HeapSize(heap))
		{
			ret_val = *(void **)DVGetItem(heap->dv, match_index);
			/* replace the element-to-remove with the tail-element */
			SwapElements(heap, match_index, last_index);
			DVPopBack(heap->dv);
			
			if (match_index < HeapSize(heap))
			{
				/* heapify up/down the replaced element */
				if (FIRST_INDEX != match_index &&
					TRUE == ElementIsBefore(heap, match_index,
										 		   PARENT_INDEX(match_index)))
				{
					HeapifyUpRec(heap, match_index);
				}
				else
				{
					HeapifyDownRec(heap, match_index);
				}
			}
		}
	}
	
	return (ret_val);
}


/************************* FindElement ****************************************/
static size_t FindElement(heap_t *heap, void *data,
						  heap_is_match_t is_match_func)
{
	size_t i = 0;
	size_t last_index = 0;
	void *heap_data = NULL;
	boolean_t is_match_found = FALSE;
	
	assert(heap);
	assert(is_match_func);
	
	last_index = LAST_INDEX(heap);
	while (i <= last_index && FALSE == is_match_found)
	{
		heap_data = *(void **)DVGetItem(heap->dv, i);
		is_match_found = is_match_func(data, heap_data);
		++i;
	}
	
	return ((TRUE == is_match_found) ? (i - 1) : i);
}


/******************************************************************************
*							HeapPeek
*******************************************************************************/
void *HeapPeek(const heap_t *heap)
{
	void *ret_val = NULL;
	
	assert(heap);
	
	if (DVSize(heap->dv) > 0)
	{
		ret_val = *(void **)DVGetItem(heap->dv, FIRST_INDEX);
	}
	
	return (ret_val);
}


/******************************************************************************
*							HeapIsEmpty
*******************************************************************************/
boolean_t HeapIsEmpty(const heap_t *heap)
{
	assert(heap);
	
	return (0 == DVSize(heap->dv));
}


/******************************************************************************
*							HeapSize
*******************************************************************************/
size_t HeapSize(const heap_t *heap)
{
	assert(heap);
	
	return (DVSize(heap->dv));
}


/******************************************************************************
*							Function
*******************************************************************************/
void HeapSetParam(heap_t *heap, const void *param)
{
	assert(heap);
	
	heap->param = (void *)param;
}


/******************************************************************************
************************	internal function	*******************************
*******************************************************************************/

/************************* SwapElements ***************************************/
static void SwapElements(heap_t *heap, size_t index_1, size_t index_2)
{
	void *element_1 = NULL;
	void *element_2 = NULL;
	size_t temp = 0;
	
	assert(heap);
	
	element_1 = DVGetItem(heap->dv, index_1);
	element_2 = DVGetItem(heap->dv, index_2);
	
	temp = *(size_t *)element_1;
	*(size_t *)element_1 = *(size_t *)element_2;
	*(size_t *)element_2 = temp;
}


/************************* ElementIsBefore ************************************/
static boolean_t ElementIsBefore(heap_t *heap, size_t index_1, size_t index_2)
{
	void *element_1 = NULL;
	void *element_2 = NULL;
	
	assert(heap);
	
	element_1 = *(void **)DVGetItem(heap->dv, index_1);
	element_2 = *(void **)DVGetItem(heap->dv, index_2);
	
	/* compare the pointers inside the vector */
	return (heap->is_before_func(element_2, element_1, heap->param));
}



/******* debug white-box tool **************/
/*********************************PrintDV**************************************/
/*static void PrintDv(heap_t *heap)*/
/*{*/
/*    size_t i = 0;*/
/*    size_t dv_size = DVSize(heap->dv);*/

/*    printf("\n\t");*/

/*    for ( ; i < dv_size ; ++i)*/
/*    {*/
/*        printf("  %3lu  ", i);*/
/*        */
/*    }*/

/*    printf("\n\t");*/

/*    for (i = 0 ; i < dv_size ; ++i)*/
/*    {*/
/*        printf("-------");*/
/*        */
/*    }*/

/*    printf("\n\t|");*/

/*    for (i = 0 ; i < dv_size ; ++i)*/
/*    {*/
/*        printf("  %d  |", **(int **)DVGetItem(heap->dv, i));*/
/*    }*/

/*    printf("\n\t");*/

/*    for (i = 0  ; i < dv_size ; ++i)*/
/*    {*/
/*        printf("-------");*/
/*        */
/*    }*/

/*    printf("\n");*/
/*}*/
