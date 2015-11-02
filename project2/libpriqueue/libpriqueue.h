/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

#define TRUE 1
#define FALSE 0

typedef BOOL unsigned int

/**
  Priqueue Data Structure
*/

typedef struct node
{
	void *data;	//I didn't think too hard about the stored data, but if the values they give us are of a random type, doesn't it make sense just to store the void ptr itself?
	struct node *next;
} node;

typedef struct _priqueue_t
{
	int size;
	struct node *head;
	struct node *tail;
} priqueue_t;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));
int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
