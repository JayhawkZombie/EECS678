/**
* Buddy Allocator
*
* For the list library usage, see http://www.mcs.anl.gov/~kazutomo/list/
*/

/**************************************************************************
* Conditional Compilation Options
**************************************************************************/
#define USE_DEBUG 0

/**************************************************************************
* Included Files
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/**************************************************************************
* Public Definitions
**************************************************************************/
#define MIN_ORDER 12
#define MAX_ORDER 20

typedef unsigned int bool;
#define true 1
#define false 0

#define PAGE_SIZE (1<<MIN_ORDER)
/* page index to address */
#define PAGE_TO_ADDR(page_idx) (void *)((page_idx*PAGE_SIZE) + g_memory)

/* address to page index */
#define ADDR_TO_PAGE(addr) ((unsigned long)((void *)addr - (void *)g_memory) / PAGE_SIZE)

/* find buddy address */
#define BUDDY_ADDR(addr, o) (void *)((((unsigned long)addr - (unsigned long)g_memory) ^ (1<<o)) \
		 + (unsigned long)g_memory)

#if USE_DEBUG == 1
#  define PDEBUG(fmt, ...) \
	fprintf(stderr, "%s(), %s:%d: " fmt,			\
		__func__, __FILE__, __LINE__, ##__VA_ARGS__)
#  define IFDEBUG(x) x
#else
#  define PDEBUG(fmt, ...)
#  define IFDEBUG(x)
#endif

/**************************************************************************
* Public Types
**************************************************************************/
typedef struct {
	struct list_head list;
	/* TODO: DECLARE NECESSARY MEMBER VARIABLES */
} page_t;

/**************************************************************************
* Global Variables
**************************************************************************/
/* free lists*/
struct list_head free_area[MAX_ORDER + 1];

/* memory area */
char g_memory[1 << MAX_ORDER];

/* page structures */
page_t g_pages[(1 << MAX_ORDER) / PAGE_SIZE];

/**************************************************************************
* Public Function Prototypes
**************************************************************************/

void buddy_init();
void *buddy_alloc(int size);
void buddy_free(void *addr);

void buddy_dump();
void buddy_dump2();

/**************************************************************************
* Local Functions
**************************************************************************/

/**
* Initialize the buddy system
*/
void buddy_init()
{
	int i;
	int n_pages = (1 << MAX_ORDER) / PAGE_SIZE;
	for (i = 0; i < n_pages; i++) {
		/* TODO: INITIALIZE PAGE STRUCTURES */
		INIT_LIST_HEAD(&g_pages[i].list);
	}

	/* initialize freelist */
	for (i = MIN_ORDER; i <= MAX_ORDER; i++) {
		INIT_LIST_HEAD(&free_area[i]);
	}

	/* add the entire memory as a freeblock */
	list_add(&g_pages[0].list, &free_area[MAX_ORDER]);
}

/**
* Allocate a memory block.
*
* @param size size in bytes
* @return memory block address
*/

void * buddy_alloc(int size) //WORKING
{
	int i;

	for (i = 0; (1 << i) < size; i++);

	if (i > MAX_ORDER) {
		return NULL;
	}
	else if (!list_empty(&free_area[i])) {
		struct list_head *block;
		block = &free_area[i];
		list_del_init(&free_area[i]);
		return block;
	}
	else {
		struct list_head *block, *buddy;
		block = buddy_alloc(1 << (i + 1));

		if (block != NULL) {
			buddy = ((struct list_head *)(((int)block) ^ (1 << i))); //the macro given was SEG FAULT for BUDDY_ADDR(block, i)
			list_add(buddy, &free_area[i]);
		}

		return block;
	}
}

/**
* Free an allocated memory block.
*
* @param addr memory block address to be freed
*/
void buddy_free(void *addr) //NOT WORKING
{

}

/**
* Print the buddy system status---order oriented
*
* print free pages in each order.
*/
void buddy_dump()
{
	int o;
	for (o = MIN_ORDER; o <= MAX_ORDER; o++) {
		struct list_head *pos;
		int cnt = 0;
		list_for_each(pos, &free_area[o]) {
			cnt++;
		}
		printf("%d:%dK ", cnt, (1 << o) / 1024);
	}
	printf("\n");
}


int main(int argc, char *argv[])
{
	char *A, *B, *C, *D;

	buddy_init();

	A = buddy_alloc(80 * 1024); //80kb
	buddy_dump();

	B = buddy_alloc(60 * 1024); //60kb
	buddy_dump();

	C = buddy_alloc(80 * 1024); //80kb
	buddy_dump();

	buddy_free(A);
	buddy_dump();

	D = buddy_alloc(32 * 1024); //32kb
	buddy_dump();

	buddy_free(B);
	buddy_dump();

	buddy_free(D);
	buddy_dump();

	buddy_free(C);
	buddy_dump();

	return 0;
}
