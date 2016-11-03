#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "linked_list.h"

#define MAX_LINE_LEN 256
#define MAX_PATH_LEN 40

// prototypes
void read_input_file(char * filename);
void releasePipe();
void requestPipe(thd *item);
void *thrFunction(void *thread_ptr);

pthread_mutex_t trans_mutex; // transmission mutex declaration
pthread_cond_t turn_cond = PTHREAD_COND_INITIALIZER; //convar initialization
struct thread *read_thds; // ptr used for array of read threads
int no_thds = 0; // total number of threads
struct timespec initial; // timespec used to calculate the time at which the program started
int trans_pipe_available = 1; // global variable indicating whether the the transmitting pipe is available

int main (int argc, char **argv) {
	int i;

	// args error checking
	if(argc == 1) {
  	printf("Expected one argument (filename)\n");
  	exit(EXIT_FAILURE);
 	}
  
  // initialize mutex and do error checking
  if (pthread_mutex_init(&trans_mutex, NULL) != 0){
        printf("Mutex init failed\n");
        return 1;
  }
	
	read_input_file(argv[1]); // reading given file
	pthread_t id_array[no_thds]; // array to hold ids of threads
	clock_gettime(CLOCK_MONOTONIC, &initial); //sets the starting point for relative machine time
	
	for (i = 0; i < no_thds; i++) { //start each thread (in order of input file)
		if(pthread_create(&id_array[i], NULL, thrFunction, (void *) &read_thds[i]) != 0) printf("Can't create thread %d\n", i); 
	}
	
	for (i = 0; i < no_thds; i++) { // wait for all threads to finish executing
    pthread_join(id_array[i], NULL);
  }

  for (i = 0; i < length(); i++) free(deleteFirst()); // delete any remaining nodes before exiting
  
	if (read_thds != NULL) free(read_thds); // deallocate dynamic memory used
	
	pthread_mutex_destroy(&trans_mutex); // destroy mutex
	pthread_cond_destroy(&turn_cond); // destroy cond var
	
	return (0);
}

// calculate how much time has passed since beginning of program
double get_current_machine_time() {
	struct timespec finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &finish); // get current clock time

	// calculate relative time
	elapsed = (finish.tv_sec - initial.tv_sec);
	elapsed += (finish.tv_nsec - initial.tv_nsec) / 1000000000.0;
	return elapsed;
}

// entry point for each thread created
void *thrFunction(void *thread_ptr) {
    thd *thread_obj = (thd*)thread_ptr;
    
    // save thread info in local variables
		int id = thread_obj->id;
		int arrival_time = thread_obj->arrival_time;
		int transmission_time = thread_obj->transmission_time;
		int priority = thread_obj->priority;
		int position = thread_obj->position;

		usleep(100000 * arrival_time); // sleep the arrival time
		
		printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%d)\n", id, (double)get_current_machine_time(), (double)transmission_time, priority);
		
		// request to transmit using pipe
		requestPipe(thread_obj);
		
    printf("Flow %2d starts its transmission at time %.2f \n", id, (double) get_current_machine_time());

    // perform transmission (sleep)
		usleep(100000 * transmission_time);

    printf("Flow %2d finishes its transmission at time %.2f\n", id, (double) get_current_machine_time());
    
    // indicate done using transmission pipe
    releasePipe();
}

void requestPipe(thd *item) {
    // lock trans mutex
		pthread_mutex_lock(&trans_mutex);

		// if the pipe is available and there's no one in the queue, return to transmit
    if (trans_pipe_available && length() == 0){
      trans_pipe_available = 0;
      pthread_mutex_unlock(&trans_mutex);
      return;
    }

		// add item in queue, sort the queue according rules
		insertFirst(item->id, item->arrival_time, item->transmission_time, item->priority, item->position);
		sort();

    // wait transmission pipe to be available and for having the first spot
		while (getFirstId() != item->id || !trans_pipe_available){
			if (getFirstId() != item->id) printf("Flow %2d waits for the finish of flow %2d \n", item->id, getFirstId());
			pthread_cond_wait(&turn_cond, &trans_mutex); // release mutex(trans_mutex), wait on turn_cond until it is signaled	
		}

    // remove executing thread from queue
		delete(item->id);
		
		// indicate the transmission pipe is being used
		trans_pipe_available = 0;

    pthread_mutex_unlock(&trans_mutex);
}

// indicates the transmission pipe is available
void releasePipe() {
    pthread_mutex_lock(&trans_mutex);
    
    // set flag to available
    trans_pipe_available = 1;
    
    // broadcast to all waiting threads that pipe is available
    pthread_cond_broadcast(&turn_cond);
    
    pthread_mutex_unlock(&trans_mutex);
}

// reads the file indicated by the user
void read_input_file(char * filename) {
	char path[MAX_PATH_LEN], line[MAX_LINE_LEN], id[MAX_LINE_LEN], 
		arrival_time[MAX_LINE_LEN], transmission_time[MAX_LINE_LEN], priority[MAX_LINE_LEN];
  FILE* input_file;
  int counter = 0;

	// create path
  snprintf(path, MAX_PATH_LEN, "%s", filename);

	// read file from path
  input_file = fopen(path, "r");

	// error checking
  if(!input_file) { 
		perror("Could not open input file");
		return;
	}

	// read until finding EOF
  while (fgets(line, MAX_LINE_LEN, input_file)) {
  	int len = strlen(line);
  	int i = 0;
  	int position = 0;
  	int status = 0;
  	
  	// look for number of threads
  	if (counter == 0) {
  		no_thds = atoi(line);
  		read_thds = malloc(no_thds * sizeof(thd)); // allocate an array for the given threads
  		if (read_thds == NULL) { // error handling
			 	perror("command malloc failed");
				exit(EXIT_FAILURE);
		 	}
  		counter++;
  		continue;
  	}

		// looking for thread id
		for (; i < len; i++) {
			if (status == 0) { 
				if (line[i] == ':') {
					status++;
					id[position] = '\0';
					position = 0;
					continue;
				} else {
					id[position] = line[i];
					position++;
				}
			}
			
			// looking for thread arrival time
			if (status == 1) { 
				if (line[i] == ',') {
					status++;
					arrival_time[position] = '\0';
					position = 0;
					continue;
				} else {
					arrival_time[position] = line[i];
					position++;
				}
			}
			
			// looking for thread transmission time
			if (status == 2) { 
				if (line[i] == ',') {
					status++;
					transmission_time[position] = '\0';
					position = 0;
					continue;
				} else {
					transmission_time[position] = line[i];
					position++;
				}
			}
			
			// looking for thread priority
			if (status == 3) { 
				if (line[i] == '\n') {
					priority[position] = '\0';
					break;
				} else {
					priority[position] = line[i];
					position++;
				}
			}
		}
		
		// populate array gathering thread's info
		read_thds[counter - 1].id = atoi(id);
		read_thds[counter - 1].arrival_time = atoi(arrival_time);
		read_thds[counter - 1].transmission_time = atoi(transmission_time);
		read_thds[counter - 1].priority = atoi(priority);
		read_thds[counter - 1].position = counter;
		counter++; // keeps track of position of thread in file
	}

  fclose(input_file);
}

