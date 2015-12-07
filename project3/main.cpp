/**
* Buddy Allocator
*
* For the list library usage, see http://www.mcs.anl.gov/~kazutomo/list/
*/

/**************************************************************************
* Conditional Compilation Options
**************************************************************************/
#define USE_DEBUG 0

typedef unsigned int BOOL;
#define TRUE 1
#define FALSE 0

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
	size_t free_blocks;
	size_t block_size;
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
	int cnt = 1;
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
/*void *buddy_alloc(int size)
{
	printf("Asking for: %d\n", size);
	int cnt = 2;

	while ((size = (size >> 1)) > 1)
		cnt = cnt + 1;

	//printf("Need index: %d\n", cnt);
	//printf("Will use block size: %d\n", (1 << cnt));
	
	int g_start, to_add, toDel;

	int index = cnt;
	page_t *page;
	if (!list_empty(&free_area[index]))
	{
		g_start = 0;
		to_add = 1;
		for (int k = 0; k < (MAX_ORDER - (index)); k++) {
			g_start += to_add;
			to_add = (1 << to_add);
		}

		//printf("Free block at index: %d No need to split\n", index);
		//printf("g_start:%d\n", g_start);
		list_del_init(&free_area[index]);
		return(&g_pages[g_start].list);
	}

	while (list_empty(&free_area[index]) && index <= MAX_ORDER)
		index++;

	//printf("Index satisfying request: %d\n", index);
	//printf("Count: %d Index: %d\n", cnt, index);
	for (int j = index; j > cnt; j--)
	{
		g_start = 0;
		to_add = 1;
		for (int k = 0; k < (MAX_ORDER-(j-1)); k++) {
			g_start += to_add;
			to_add = (1 << to_add);
		}

		//toDel = g_start / 2;

		//printf("g_start:%d\n", g_start);
		//list_add(&g_pages[g_start].list, &free_area[j - 1]);
		//list_add_tail(&g_pages[g_start + 1].list, &free_area[j - 1]);

		////list_del_init(&free_area[j - 1]);
		//list_del_init(&g_pages[toDel].list);

		//printf("%i \n", g_start);
		//buddy_dump();
	}
	//buddy_dump();
	//exit(0);
	//Look in the list with index max(cnt - MIN_ORDER, 0)
	//toDel /= 2;
	list_del_init(&g_pages[g_start].list);

	return(&g_pages[g_start]);

	//return NULL;
}*/

void *buddy_alloc(int size) {
	int i;

	for (i = 0; (1 << i) < size; i++);
	//printf("i: %d\n", i);
	if (i > MAX_ORDER) {
		//printf("NO SPACE AVAILABLE");
		return NULL;
	}
	else if (!list_empty(&free_area[i])) {
		printf("We have a free block at level: %d\n", i);
		struct list_head *block = &free_area[i];
		//free_area[i] = *(struct list_head *)&free_area[i];
		
		return block;
	}
	else {
		printf("Need to split\n");
		struct list_head *block, *buddy;
		block = buddy_alloc(1 << (i + 1));
		printf("AFTER SPLIT\n");

		if (block) {
			printf("Block got split.\n");
			buddy = BUDDY_ADDR(block, i);
			free_area[i] = *buddy;
			list_add(block, &free_area[i - 1]);
		}

		return block;
	}
}

/**
* Free an allocated memory block.
*
* @param addr memory block address to be freed
*/
void buddy_free(void *addr)
{
	/* TODO: IMPLEMENT THIS FUNCTION */
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

	A = buddy_alloc(80 * 1024);
	buddy_dump();

	B = buddy_alloc(60 * 1024);
	buddy_dump();

	C = buddy_alloc(80 * 1024);
	buddy_dump();

	buddy_free(A);
	buddy_dump();

	D = buddy_alloc(32 * 1024);
	buddy_dump();

	buddy_free(B);
	buddy_dump();

	buddy_free(D);
	buddy_dump();

	buddy_free(C);
	buddy_dump();

	return 0;
}
