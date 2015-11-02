/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->size = 0;
	q->head = q->tail = NULL;
	
	//...why do we have a comparer? Isn't this an empty queue?
	
	return q;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	struct node* n = {
		.data = ptr,
		.next = NULL	
	};

	if(q->head == NULL) {
		q->head = n;
	}
	else {
		q->tail->next = n;
	}
	
	int index = q->size;
	
	q->tail = n;
	q->size++;
	
	return index;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	return q->head;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	struct node *n = q->head;
	q->head = n->next;
	
	//I want to free n here but they're having us return it... maybe it can be freed after this is called
	
	return n;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if(q->size-1 < index) {
		printf("Specified index does not exist\n");
		return NULL;
	}
	
	struct node *n = q->head;
	for(int i=0; i<index; i++) {
		n = n->next;
	}
	
	return n;
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	int count = 0;
	struct node* prev = NULL;
	struct node *n = q->head;
	
	while(n != NULL) {
		if(n->data == ptr) {//haven't run this code but this might just be comparing memory addresses... if it is, we would need to dereference the pointers
							//to compare their values, but when you dereference a void ptr don't you have to cast it? And we wouldnt know the types here...
			if(prev == NULL) {
				q->head = q->head->next;
				free(n);
				n = q->head;
			}
			else {
				prev->next = n->next;
				free(n);
				n = prev->next;
			}
			
			count++;
		}
		else {
			prev = n;
			n = n->next;
		}
	}
	
	return count;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if(q->size-1 < index) {
		printf("Specified index does not exist\n");
		return NULL;
	}
	
	struct node* prev = NULL;
	struct node* n = q->head;
	
	for(int i=0; i<index; i++) {
		prev = n;
		n = n->next;
	}
	
	prev->next = n->next;
	q->size--;
	
	//I want to free n here but they're having us return it so I need to keep it around until the function finishes.. maybe it can be freed after this is called
	
	return n;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	while ((curr = q->head) != NULL) {
		q->head = q->head->next;
		free(curr);
	}
	
	free(q->size);
	free(q);
}
