/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

typedef struct core
{
  int active;
} core_t;


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
  int job_num;
  int arrival_time;
  int running_time;
  int priority;
  int start_time;
  int head;
  int remaining_time;
  int paused;

  /* Assign some variables for the managing of statistics - like waiting time */

} job_t;


scheme_t CURRENT_SCHEME;
priqueue_t QUEUE;
float waiting_time;
float turnaround_time;
float response_time;
int total_num_jobs;

int start;
int time_of_start = -1;
int end;

/* COMPARISON FUNCTIONS */

int FCFS_COMPARE(const void *a, const void *b){
  return(
      ((job_t *)a)->arrival_time - ((job_t *)b)->arrival_time
    );
}

int SJF_COMPARE(const void *a, const void *b){
  if ( ((job_t *)b)->head == 1)
  {
    return (1);
  }
  else
    return (
        ((job_t *)a)->running_time - ((job_t *)b)->running_time
      );
}

int PSJF_COMPARE(const void *a, const void *b)
{
  return (
      ((job_t)a)->time_remaining - ((job_t)b)->time_remaining
    );
}

int PRI_COMPARE(const void *a, const void *b)
{    
  if( ((job_t *)b)->head == 1 )
    {
      return 1;
    }
  else
  {
    return (
        ((job_t *)a)->priority - ((job_t *)b)->priority
      );
  }
}

int PPRI_COMPARE(const void *a, const void *b)
{
  return ( ((job_t *)a)->priority - ((job_t *)b)->priority );
}

int RR_COMPARE(const void *a, const void *b)
{
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

  switch(scheme)
  {
    case FCFS:
      CURRENT_SCHEME = FCFS;
      priqueue_init(&QUEUE, FCFS_COMPARE);
      break;
    case SJF:
      CURRENT_SCHEME = SJF;
      priqueue_init(&QUEUE, SJF_COMPARE);
      break;
    case PSJF:
      CURRENT_SCHEME = PSJF;
      priqueue_init(&QUEUE, PSJF_COMPARE);
      break;
    case PPRI:
      CURRENT_SCHEME = PPRI;
      priqueue_init(&QUEUE, PPRI_COMPARE);
      break;
    case PRI:
      CURRENT_SCHEME = PRI;
      priqueue_init(&QUEUE, PRI_COMPARE);
      break;
    case RR:
      CURRENT_SCHEME = RR;
      priqueue_init(&QUEUE, RR_COMPARE);
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
  job_t *incoming_job = malloc(sizeof(job_t));
  incoming_job->job_num = job_number;
  incoming_job->arrival_time = time;
  incoming_job->running_time = running_time;
  incoming_job->priority = priority;
  incoming_job->time_remaining = running_time;
  incoming_job->start_time = -1;
  incoming_job->head = 0;

  total_num_jobs += 1;

  job_t *peek = priqueue_peek(&QUEUE);


  if (CURRENT_SCHEME == FCFS || CURRENT_SCHEME == SJF || CURRENT_SCHEME == PRI)
  {
    priqueue_offer(&QUEUE, incoming_job);

    if (peek != NULL)
    {
      incoming_job -> head = 1;
      incoming_job -> start_time = time;
      return 0;
    }

    else
      return -1;
  }

  else if(CURRENT_SCHEME == PSJF)
  {
    if(peek != NULL)
    {
      peek->remaining_time = peek->remaining_time - (time - peek->paused); //Subtract the last amount of time used from the remaining time
    }

    priqueue_offer(&QUEUE, incoming_job);

    if(peek == NULL)
    {
      //We are going to be adding the first (or only) job to the queue
      incoming_job->start_time = time;
      incoming_job->paused = time;
      incoming_job->head = 1;

      //I feel like I'm missing something here...  what happens if start_time == time?
      //I don't know

      return 0;
    }
    else if(peek != NULL && (peek->remaining_time > running_time))
    {
      incoming_job->start_time = time;
      incoming_job->paused = time;
      incoming_job->head = 1;
      //______
      return 0;
    }
    else
    {
      return -1;
    }
  }

  else if(CURRENT_SCHEME == PPRI)
  {
    priqueue_offer(&QUEUE,  incoming_job);

    if(peek == NULL)
    {
      incoming_job->start_time = time;
      incoming_job->paused = time;
      return 0;
    }

    else if(peek != NULL && (peek->priority > priority))
    {
      incoming_job->start_time = time;
      incoming_job->paused = time;
      incoming_job->head = 1;

      //______

      return 0;
    }
    else
      return -1;
  }

  else
  {
    priqueue_offer(&QUEUE, incoming_job)

    if(peek == NULL)
    {
      incoming_job->start_time = time;
      return 0;
    }
    return -1;
  }


	return -1;
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


  job_t poll = priqueue_poll(&QUEUE);

  waiting_time += ((time - poll->arrival_time) - poll->running_time);
  turnaround_time += time - poll->arrival_time;
  response_time += poll->start_time - (poll->arrival_time);

  free(poll);

  job_t *peek = priqueue_peek(&QUEUE);

  if(CURRENT_SCHEME == FCFS || CURRENT_SCHEME == SJF || CURRENT_SCHEME == PRI)
  {
    if(peek == NULL)
      return -1;
    else
    {
      peek->head = 1;
      peek->start_time = time;
      return (peek->job_num);
    }
  }

  else if (CURRENT_SCHEME == PSJF)
  {
    if (peek == NULL)
      return -1;
    else
    {
      peek->paused = time;
      peek->head = 1;
      //_____
      return(peek->job_num);
    }
  }

  else if (CURRENT_SCHEME == PPRI)
  {
    if (peek == NULL)
      return -1;
    else
    {
      peek->paused = time;
      peek->head = 1;
      //_____
      return(peek->job_num);
    }
  }

  else
  {
    if(peek == NULL)
      return -1;
    else
    {
      peek->paused = time;
      peek->head = 1;

      //______
      return(peek->job_num);
    }
  }

	return -1;
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


  job_t *peek = priqueue_peek(&QUEUE);

  if(peek == NULL)
    return -1;
  else
  {
    priqueue_offer(&QUEUE, peek);
    peek = priqueue_peek(&QUEUE);

    if(peek == NULL)
      return -1;
    else
    {
      return (peek->job_num);
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
  return( (float)(waiting_time) / total_num_jobs );
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return ( (float)(turnaround_time) / total_num_jobs );
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return ( (float)(response_time) / total_num_jobs );
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

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
