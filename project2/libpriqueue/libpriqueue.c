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
	q->head = NULL;
	q->cmp = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	struct node *temp;
	struct node *previous;
	struct node *insert = malloc(sizeof(struct node));

	insert->data = ptr;
	insert->next = NULL;

	//check for other nodes
	if (q->head == NULL) {
		q->head = insert;
	}
	//compare the new node to the first node
	else if(q->cmp(insert->data, q->head->data) < 0) {
		insert->next = q->head;
		q->head = insert;
	}
	else {
		previous = q->head;
		temp = q->head->next;

		//determine where the new node needs to be inserted 
		while(temp != NULL && q->cmp(insert->data, temp->data) > 0)
		{
			previous = temp;
			temp = temp->next;
		}

		//insert the node at the position
		if(temp == NULL)
		{
			previous->next = insert;
		}
		else
		{
			insert->next = temp;
			previous->next = insert;
		}
	}

	return priqueue_size(q)-1;
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
	return q->head->data;
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
	void* value = q->head->data;
	q->head = q->head->next;
	
	return value;
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
	int i = 0;
	struct node *n = malloc(sizeof(struct node));
	n = q->head;

	while(i != index && n != NULL) {
		n = n->next;

		i++;
	}
	
	return n->data;
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
	int numRemoved = 0;
	struct node* previous, *temp;

	temp = q->head;
	
	while(temp != NULL) {
		if(q->cmp(temp->data == ptr) == 0) {
			if(previous == NULL) {
				q->head = q->head->next;
				temp = q->head;
			}
			else {
				previous->next = temp->next;
				temp = previous->next;
			}
			
			numRemoved++;
		}
		else {
			previous = temp;
			temp = temp->next;
		}
	}

	return numRemoved;
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
	int i = 0;
	struct node* prev = malloc(sizeof(struct node));
	struct node* n = malloc(sizeof(struct node));
	n = q->head;
	
	while(i != index && n != NULL) {
		prev = n;
		n = n->next;

		i++;
	}
	
	if(n != NULL) {
		prev->next = n->next;
	}
	
	return n->data;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	int count = 0;
	struct node* n = malloc(sizeof(struct node));
	n = q->head;

	while(n != NULL) {
		n = n->next;
		count++;
	}

	return count;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	struct node* curr = malloc(sizeof(struct node));

	while ((curr = q->head) != NULL) {
		q->head = q->head->next;
		free(curr);
	}

}
