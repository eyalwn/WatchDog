/******************************************************************************
*	File name:		dynamic_vector.c
*	Developer:		Eyal Weizman
*	Date:			05 Feb 2019
*	Description:	dynamic_vector vsource file
*******************************************************************************/
#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc, realloc, free */
#include <assert.h> /* assert */
#include <string.h> /* memmove */

#include "dynamic_vector.h"

/*** enums ***/
enum errors
{
	FAIL = -1,
	SUCCESS
};

/*** structs ***/
struct dynamic_vector
{
	size_t element_size;
	size_t capacity; /* max size in bytes */
	size_t size; /* the occupied size in byets */
	void *vector; /* pointer to the vector's base */
};

/******************************************************************************
*								DV_Create
*******************************************************************************/
dv_t *DVCreate(size_t capacity, size_t element_size)
{
	dv_t *dv = NULL;
	
	dv = (dv_t *) malloc(sizeof(dv_t));
	if(NULL == dv)
	{
		return (NULL);
	}
	
	dv->vector = (void *) malloc(element_size * capacity);
	if(NULL == dv->vector)
	{
		free(dv);
		return (NULL);
	}
	
	dv->element_size = element_size;
	dv->capacity = element_size * capacity;
	dv->size = 0;
	
	return (dv);
}


/******************************************************************************
*								DV_Destroy
*******************************************************************************/
void DVDestroy(dv_t *dv)
{
	assert(dv);
	
	dv->element_size = 0;
	dv->capacity = 0;
	dv->size = 0;
	
	free(dv->vector);
	dv->vector = NULL;
	
	free(dv);
	dv = NULL;
}


/******************************************************************************
*								DV_PushBack
*******************************************************************************/
int DVPushBack(dv_t *dv, const void *element)
{
	assert(dv);
	assert(element);
	
	/* checks wheather more space is needed */
	if (dv->size == dv->capacity)
	{
		/* duplicates the current capacity via DVReserve function + checks */
		if (FAIL == DVReserve(dv, (dv->capacity / (dv->element_size))))
		{
			return (FAIL);
		}
	}
	
	/* copies from the element-pointer to top of the vector */
	memcpy((((char *) dv->vector) + dv->size) , element, dv->element_size);
	
	dv->size += dv->element_size;
	
	return (SUCCESS);
}


/******************************************************************************
*								DVPopBack
*******************************************************************************/
int DVPopBack(dv_t *dv)
{
	size_t temp_capacity = 0;
	void *temp_vector = NULL;
	
	assert(dv);

	if (0 == dv->size)
	{
		return (FAIL);
	}
	
	/* case size is now quarter of the capacity - reduce capacity in 1/2 */
	if (dv->size <= (dv->capacity / 4))
	{
		/* prepare a capacity size wich can be devided in element_size */
		temp_capacity = dv->capacity / 2;
		temp_capacity -= (temp_capacity % dv->element_size);
		
		temp_vector = realloc(dv->vector, temp_capacity);
		if (NULL == temp_vector)
		{
			return (FAIL);
		}
		
		dv->vector = temp_vector;
		dv->capacity = temp_capacity;
	}
	
	dv->size -= dv->element_size;
	
	return (SUCCESS);
}


/******************************************************************************
*								DVSize
*******************************************************************************/
size_t DVSize(const dv_t *dv)
{
	assert(dv);
	
	return ((dv->size) / dv->element_size);
}


/******************************************************************************
*								DVCapacity
*******************************************************************************/
size_t DVCapacity(const dv_t *dv)
{
	assert(dv);
	
	return ((dv->capacity) / dv->element_size);
}


/******************************************************************************
*								DVGetItem
*******************************************************************************/
void *DVGetItem(const dv_t *dv, size_t index)
{
	/* case the index is out of range */
	if ((index * (dv->element_size)) >= dv->size)
	{
		return (NULL);
	}
	
	return (((char *) dv->vector) + index * (dv->element_size));
}


/******************************************************************************
*								DVReserve
*******************************************************************************/
int DVReserve(dv_t *dv, size_t additional_size)
{
	void *temp_vector = NULL;
	
	/* temp gets the new address so that in case of failure - the original 
	 * vector won't get lost
	 */
	temp_vector = realloc(dv->vector,
					(dv->capacity + (additional_size * (dv->element_size))));
					
	if (NULL == temp_vector)
	{
		return (FAIL);
	}
	
	dv->vector = temp_vector;
	dv->capacity += additional_size * (dv->element_size);
	
	return (SUCCESS);
}


