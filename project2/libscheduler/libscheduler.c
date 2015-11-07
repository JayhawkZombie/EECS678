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

	return jobA->run_time - jobB->run_time;
}

int PSJF_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	return jobA->time_remaining - jobB->time_remaining;
}
//This one
int PRI_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	return jobA->priority - jobB->priority;
}
//and this one were the same I when I got your changes I think... maybe not and you still have it?
int PPRI_COMPARE(const void *a, const void *b) {
	job_t* jobA = (job_t*) a;
	job_t* jobB = (job_t*) b;

	return jobA->priority - jobB->priority;
}
//Not yet implemented
int RR_COMPARE(const void *a, const void *b)
{
  return 0;
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
	int num_cores = cores;
	core_t* core_list = malloc(cores * sizeof(core_t));

	for(int i=0; i<cores; i++) {
		core_list[i].active = 0;
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
			priqueue_init(QUEUE, PPRI_COMPARE);
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
	new_job->arrival_time 	= time;
	new_job->priority 		= priority;
	new_job->run_time 		= running_time;
	new_job->time_remaining = running_time;

	num_jobs++;
	

	//Check each core, looking for one that is inactive
	int core_index = -1;
	
	for(int i=0; i<num_cores; i++) {
		if(!core_list[i].active) {
			core_index = i;
			break;
		}
	}
	
	//If the job is within the first core_num positions of the queue after being offered, it needs to be scheduled.

	//No matter what position in the queue it is sorted to, if it is within this range,
	//the last index of the queue within executable range of our cores needs to be paused and the core reassigned.


	//Here all the cores are running jobs, so we need to offer and check the queue
	if(core_index == -1) {
		job_t* last = (job_t*) priqueue_at(QUEUE, num_cores-1);
	
		priqueue_offer(QUEUE, new_job);

		//Check to see if this job is in executable range
		for(int i=0; i<num_cores; i++) {
			if((job_t*) priqueue_at(QUEUE, i) == new_job) {

				core_index 			 = last->core_id;
				new_job->start_time  = time;
				last->core_id 		 = -1;
				last->pause_time 	 = time;
				last->time_remaining = last->time_remaining - (time - last->start_time);

			}
		}
	}
	
	new_job->core_id = core_index; //assign the job to the determined core index

	//If we have a core index make sure it's active
	if(core_index != -1) {
		core_list[core_index].active = 1;
	}

	return core_index;

	//Old code below - granted the first case has been tinkered with and it is a switch now. Left for reference.
	
/*	switch(CURRENT_SCHEME) {
		case FCFS: case SJF: case PRI:
			if (peek == new_job) {//the job was sorted to the front of the queue, so schedule it
				new_job->start_time = time;
			}
			else {//Here, the peeked job after the offer was the same as the currently scheduled job
				core_index -1;
			}
		break;
		case PSJF:
			//peek will always exist since we pass a job to the queue immediately before peeking 
			peek->time_remaining = peek->time_remaining - (time - peek->pause_time); //Subtract the last amount of time used from the remaining time
			
			if(peek == NULL)
			{
				//We are going to be adding the first (or only) job to the queue
				new_job->start_time = time;
				new_job->pause_time = time;

				//I feel like I'm missing something here...  what happens if start_time == time?
				//I don't know
			}
			else if(peek != NULL && (peek->time_remaining > run_time))
			{
				new_job->start_time = time;
				new_job->pause_time = time;
			}
			else {
				core_index = -1;
			}
		break;
		case PPRI:
			if(peek == NULL) {
				new_job->start_time = time;
				new_job->pause_time = time;
			}
			else if(peek != NULL && (peek->priority > priority)) {
				new_job->start_time = time;
				new_job->pause_time = time;
			}
			else {
				core_index = -1;
			}
		break;
		case RR:
			if(peek == NULL)
			{
				new_job->start_time = time;
				return 0;
			}
			return -1;
		break;
		default:
			fprintf(stderr, "No current scheme set for scheduling.\n");
		break;
	}
*/
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
	job_t* finished;
	job_t* temp;

	while(i<num_cores && !finished) {
		temp = (job_t*) priqueue_at(QUEUE, i);

		if(temp->job_id == job_number) {
			finished = priqueue_remove_at(QUEUE, i);
			temp = NULL;
		}
	}

	//Increment global statistics
	waiting_time += ((time - finished->arrival_time) - finished->run_time);
	turnaround_time += time - finished->arrival_time;
	response_time += finished->start_time - (finished->arrival_time);

	free(finished);

	//Now check if there are still as many jobs as we have cores even after removing.
	//If there were less jobs than cores after removing, then nothing has been waiting in queue.
	if(priqueue_size(QUEUE) >= num_cores) {
		job_t* wake_job = (job_t*) priqueue_at(QUEUE, num_cores-1);
		wake_job_id = wake_job->job_id;

		wake_job->start_time = time;
		wake_job->pause_time = -1;
	}

	return wake_job_id;






  //Old code below. Left for reference.

/*
  job_t *peek = priqueue_peek(QUEUE);

  if(CURRENT_SCHEME == FCFS || CURRENT_SCHEME == SJF || CURRENT_SCHEME == PRI)
  {
    if(peek == NULL)
      return -1;
    else
    {
      peek->start_time = time;
      return (peek->job_id);
    }
  }

  else if (CURRENT_SCHEME == PSJF)
  {
    if (peek == NULL)
      return -1;
    else
    {
      peek->pause_time = time;

      return(peek->job_id);
    }
  }

  else if (CURRENT_SCHEME == PPRI)
  {
    if (peek == NULL)
      return -1;
    else
    {
      peek->pause_time = time;

      return(peek->job_id);
    }
  }

  else
  {
    if(peek == NULL)
      return -1;
    else
    {
      peek->pause_time = time;

      return(peek->job_id);
    }
  }

	return -1;*/
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


  job_t *peek = priqueue_peek(QUEUE);

  if(peek == NULL)
    return -1;
  else
  {
    priqueue_offer(QUEUE, peek);
    peek = priqueue_peek(QUEUE);

    if(peek == NULL)
      return -1;
    else
    {
      return (peek->job_id);
    }
  }

	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  return( (float)(waiting_time) / num_jobs );
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return ( (float)(turnaround_time) / num_jobs );
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return ( (float)(response_time) / num_jobs );
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	priqueue_destroy(&QUEUE);
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

}
