/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/

typedef struct _job_t {
  int job_id, core_id;
  int arrival_time, run_time, priority;
  int start_time, time_remaining, pause_time;
  int responded;
} job_t;

typedef struct _core_t {
  int active;
} core_t;

core_t* core_list;
priqueue_t* QUEUE;
scheme_t CURRENT_SCHEME;

int num_jobs;
int num_cores;

float waiting_time;
float turnaround_time;
float response_time;


/* COMPARISON FUNCTIONS */

int FCFS_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	return jobA->arrival_time - jobB->arrival_time;
}

int SJF_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	int diff = jobA->run_time -jobB->run_time;

	if(diff == 0) {
		diff = jobA->arrival_time - jobB->arrival_time;
	}

	return diff;
}

int PSJF_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	int diff = jobA->time_remaining - jobB->time_remaining;

	if(diff == 0) {
		diff = jobA->arrival_time - jobB->arrival_time;
	}

	return diff;
}

int PRI_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	int diff = jobA->priority - jobB->priority;

	if(diff == 0) {
		diff = jobA->arrival_time - jobB->arrival_time;
	}

	return diff;
}

//Not yet implemented
int RR_COMPARE(const void *a, const void *b) {
  return 1;
}


/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
	num_jobs = 0;

	QUEUE = malloc(sizeof(priqueue_t));

	num_cores = cores;
	core_list = malloc(cores*sizeof(core_t));

	for(int i=0; i<cores; i++) {
		core_list[i].active = -1;
	}

	switch(scheme) {
		case FCFS:
			CURRENT_SCHEME = FCFS;
			priqueue_init(QUEUE, FCFS_COMPARE);
		break;
		case SJF:
			CURRENT_SCHEME = SJF;
			priqueue_init(QUEUE, SJF_COMPARE);
		break;
		case PSJF:
			CURRENT_SCHEME = PSJF;
			priqueue_init(QUEUE, PSJF_COMPARE);
		break;
		case PPRI:
			CURRENT_SCHEME = PPRI;
			priqueue_init(QUEUE, PRI_COMPARE);
		break;
		case PRI:
			CURRENT_SCHEME = PRI;
			priqueue_init(QUEUE, PRI_COMPARE);
		break;
		case RR:
			CURRENT_SCHEME = RR;
			priqueue_init(QUEUE, RR_COMPARE);
		break;
	}
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	//So, Let's declare a new job that we will add to the queue
	job_t *new_job = malloc(sizeof(job_t));

	new_job->job_id 		= job_number;
	new_job->core_id		= -1;
	new_job->arrival_time 	= time;
	new_job->priority 		= priority;
	new_job->run_time 		= running_time;
	new_job->time_remaining = running_time;
	new_job->pause_time		= time;
	new_job->start_time 	= -1;
	new_job->responded  	= -1;

	num_jobs++;

	//Check each core, looking for one that is inactive
	int core_index = -1;

	for(int i=0; i<num_cores; i++) {
		if(core_list[i].active == -1) {
			core_index = i;
			break;
		}
	}

	if(core_index != -1) {
		priqueue_offer(QUEUE, new_job);

		new_job->core_id = core_index;
		new_job->responded = 1;
		new_job->start_time = time;

		core_list[core_index].active = 1;
	}
	else if(CURRENT_SCHEME == PPRI || CURRENT_SCHEME == PSJF) {

		job_t* temp = NULL;

		for(int i=0; i<num_cores; i++) {
			temp = (job_t*) priqueue_at(QUEUE, i);
			temp->time_remaining = temp->time_remaining - (time - temp->start_time);
			temp->start_time = time;
		}

		priqueue_offer(QUEUE, new_job);

		for(int i=0; i<num_cores; i++) {
			if((job_t*) priqueue_at(QUEUE, i) == new_job) {
				core_index = temp->core_id;

				new_job->core_id = core_index;
				new_job->start_time = time;
				new_job->pause_time = 0;
				new_job->responded = 1;

				temp->core_id = -1;
				temp->pause_time = time;
			}
		}
	}
	else {
		priqueue_offer(QUEUE, new_job);
	}

	return core_index;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	int wake_job_id = -1;

	int i = 0;
	job_t* finished = NULL;
	job_t* wake_job = NULL;
	job_t* temp = NULL;

	while(i<priqueue_size(QUEUE) && !finished) {
		temp = (job_t*) priqueue_at(QUEUE, i);

		if(temp->job_id == job_number) {
			finished = (job_t*) priqueue_remove_at(QUEUE, i);
			temp = NULL;
		}

		i++;
	}

	turnaround_time += (time - finished->arrival_time);

	free(finished);

	i = 0;
	while(i<priqueue_size(QUEUE) && !wake_job) {
		temp = (job_t*) priqueue_at(QUEUE, i);
		if(temp->core_id == -1) {
			wake_job = (job_t*) priqueue_at(QUEUE, i);
			temp = NULL;
		}

		i++;
	}

	if(wake_job) {
		wake_job_id = wake_job->job_id;

		waiting_time += time - wake_job->pause_time;

		wake_job->core_id	 = core_id;
		wake_job->start_time = time;
		wake_job->pause_time = 0;

		if(wake_job->responded == -1) {
			wake_job->responded = 1;
			response_time += time - wake_job->arrival_time;
		}
	}
	else {
		core_list[core_id].active = -1;
	}

	return wake_job_id;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	int wake_job_id = -1;
	job_t* expire_job = NULL;
	job_t* wake_job = NULL;
	job_t* temp = NULL;

	int i = 0;
	while(i<priqueue_size(QUEUE) && !expire_job) {
		temp = (job_t*) priqueue_at(QUEUE, i);
		if(temp->core_id == core_id) {
			expire_job = (job_t*) priqueue_remove_at(QUEUE, i);
			temp = NULL;
		}

		i++;
	}

	if(expire_job) {
		expire_job->core_id = -1;
		expire_job->pause_time = time;
		expire_job->time_remaining = expire_job->time_remaining - (time - expire_job->start_time);

		priqueue_offer(QUEUE, expire_job);

		i = 0;
		while(i<priqueue_size(QUEUE) && !wake_job) {
			temp = (job_t*) priqueue_at(QUEUE, i);
			if(temp->core_id == -1) {
				wake_job = (job_t*) priqueue_at(QUEUE, i);
				temp = NULL;
			}

			i++;
		}

		if(wake_job) {
			wake_job_id = wake_job->job_id;

			waiting_time += time - wake_job->pause_time;

			wake_job->core_id = core_id;
			wake_job->start_time = time;
			wake_job->pause_time = -1;

			if(wake_job->responded == -1) {
				wake_job->responded = 1;
				response_time += time - wake_job->arrival_time;
			}
		}
		else {
			core_list[core_id].active = -1;
		}
	}
	else {
		core_list[core_id].active = -1;
	}

	return wake_job_id;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  return( (float)(waiting_time)/num_jobs );
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return ( (float)(turnaround_time)/num_jobs );
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return ( (float)(response_time)/num_jobs );
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	priqueue_destroy(QUEUE);
	free(core_list);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	job_t *job;

	for(int i=0; i<priqueue_size(QUEUE); i++) {
		job = (job_t*) priqueue_at(QUEUE, i);
		printf("%d(%d)[%d] ", job->job_id, job->core_id, job->time_remaining);
	}
}
