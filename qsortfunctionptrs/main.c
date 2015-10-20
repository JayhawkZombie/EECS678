// qsortftnptrs.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>

#include <stdio.h>

//#include <ostream>

struct process
{
	int pid;
	int arrTime;
	int priority;
} process;

/* void qsort(void *base, size_t nmemb, size_t size, int (*compar)  (const void *, const void *) ) */
//BASE ADDRESS base, nmemb number of elements, size declares size of elements, compar is the comparison function
/* Declare the function pointer */
typedef int (*comparer) (const void * a, const void * b);


int sortArrival(const void * a, const void * b)
{
	struct process p = *(struct process *)a;
	struct process q = *(struct process *)b;

	return(p.arrTime - q.arrTime);
}

int sortPriority(const void * a, const void * b)
{
	struct process p = *(struct process *)a;
	struct process q = *(struct process *)b;

	if (p.priority == q.priority)
	{
		//Well, now we have to sort by arrival time

		return(p.arrTime - q.arrTime);
	}
	return q.priority - q.priority;
}



int main(int argc, char* argv[])
{

	//Read text file
	//Push data into process struct elements

	struct process processArray[7];
	int pids[] = { 5, 2, 7, 3, 1, 4, 6 };
	int arrTimes[] = {10, 4, 14, 6, 2, 8, 12};
	int priorities[] = {3, 0, 0, 1, 1, 2, 3};

	//The size of the process structure
	int sizeOfProcess = sizeof(struct process);

	//Initialize values
	for (int i = 0; i < 7; i++)
	{
		processArray[i].pid = pids[i];
		processArray[i].arrTime = arrTimes[i];
		processArray[i].priority = priorities[i];
	}
	
	//sort!
	qsort(processArray, 7, sizeOfProcess, sortPriority);

	printf("Sorted by priority\n");
	for (int i = 0; i < 7; i++)
	{
		printf("PID: %3d     Arrival Time: %3d    Priority: %3d\n", processArray[i].pid, processArray[i].arrTime, processArray[i].priority);
	}

	qsort(processArray, 7, sizeOfProcess, sortArrival);

	printf("\n\nSorted by arrival time\n");
	for (int i = 0; i < 7; i++)
	{
		printf("PID: %3d     Arrival Time: %3d    Priority: %3d\n", processArray[i].pid, processArray[i].arrTime, processArray[i].priority);
	}

	return 0;
}

