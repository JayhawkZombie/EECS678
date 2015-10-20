#include <stdlib.h>
#include <stdio.h>


/* A structure to hold the information - I like this organization */
struct process
{
	int pid;
	int arrTime;
	int priority;
};

/* Sort based on arrival time of the process (two arrival times should never be equal) */
int sortArrival(const void * a, const void * b)
{
	struct process p = *(struct process *)a;
	struct process q = *(struct process *)b;

	return(p.arrTime - q.arrTime);
}

/* Sort based on a priority number - these can match
   If two priority numbers match, sort them then based on the arrival time (FCFS)
 */
int sortPriority(const void * a, const void * b)
{
	struct process p = *(struct process *)a;
	struct process q = *(struct process *)b;

	if(p.priority == q.priority)
	{
		//Well, now sort based on arrival time
		return (p.arrTime - q.arrTime);
	}

	return (p.priority - q.priority);
}

int main(int argc, char *argv[])
{
	/* We kind of already know there will be 7 processes in the text file - can easily be extended
	to N numbers of processes */
	 struct process processArray[7];

	 /* Integer array to hold all 21 integers (pid, arrTime, priority#) * 7 - 1 set of 3 for each process */
	 int processes[7 * 3];

	 /* A buffer to hold the line read from the file */
	 char line_buffer[BUFSIZ];
	 int line_number = 0;

	 /* Declate the file */
	 FILE *in;

	 /* Attempt to open the file - inform the user if we were unable to do so */
	 in = fopen("process.txt", "r");
	 if(!in)
	 {
	 	printf("Could not open file %s for reading", "process.txt");
	 	exit(-1);
	 }

	 /* We successfully opened the file, so read the numbers in the file into the processes array */
	 while(fgets(line_buffer, sizeof(line_buffer), in))
	 {
	 	processes[line_number++] = atoi(line_buffer);
	 }

	 /* Set up arrays to hold information about the processes */
	 int pids[7] = {0};
	 int arrTimes[7] = {0};
	 int priorities[7] = {0};

	 /* Fill up the information arrays */
	 for(int i = 0; i < 7; i++)
	 {
	 	pids[i] = processes[3*i];
	 	arrTimes[i] = processes[3*i + 1];
	 	priorities[i] = processes[3*i + 2];
	 }

	 /* The size of the "process" structure */
	 int sizeOfProcess = sizeof(struct process);

	 /* Fill up the processesArray array with the information stored in the other array */
	 for(int i = 0; i < 7; i++)
	 {
	 	processArray[i].pid = pids[i];
	 	processArray[i].arrTime = arrTimes[i];
	 	processArray[i].priority = priorities[i];
	 }

	 /* Sort! */
	 qsort(processArray, 7, sizeOfProcess, sortPriority);

	 /* Output results */
	 printf("Sorted by priority\n");

	 for (int i = 0; i < 7; i++)
	{
		printf("PID: %3d     Arrival Time: %3d    Priority: %3d\n", processArray[i].pid, processArray[i].arrTime, processArray[i].priority);
	}

	/* Sort again! */
	 qsort(processArray, 7, sizeOfProcess, sortArrival);

	 /* Output results again! */
	 printf("\n\nSorted by arrival time\n");

	 for (int i = 0; i < 7; i++)
	{
		printf("PID: %3d     Arrival Time: %3d    Priority: %3d\n", processArray[i].pid, processArray[i].arrTime, processArray[i].priority);
	}

	/* We're done - Finally :) */
	return 0;
}
