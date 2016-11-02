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

void read_input_file();
void *thread_start(void *thread_ptr);
void thread_transmission(int id, int arrival_time, int transmission_time, int priority, int position);

pthread_t * thread_ids;
pthread_mutex_t ll_mutex; // read-write linked list mutex declaration
pthread_mutex_t trans_mutex; // transmission mutex declaration
pthread_cond_t turn_cond = PTHREAD_COND_INITIALIZER; //convar initialization
struct thread *read_thds;
int no_thds = 0;
long start_time;
struct timespec initial;

int main (int argc, char **argv) {
	int i;
	
	if (pthread_mutex_init(&ll_mutex, NULL) != 0){ //mutex initialization
        printf("Mutex init failed\n");
        return 1;
  }
  
  if (pthread_mutex_init(&trans_mutex, NULL) != 0){ //mutex initialization
        printf("Mutex init failed\n");
        return 1;
  }
	
	read_input_file();
	pthread_t id_array[no_thds];
	thread_ids = id_array;
	clock_gettime(CLOCK_MONOTONIC, &initial); //sets the starting point for relative machine time
	//start_time = round(initial.tv_nsec / 1.0e6);
	for (i = 0; i < no_thds; i++) {
		if(pthread_create(&id_array[i], NULL, thread_start, (void *) &read_thds[i]) != 0) printf("Can't create thread %d\n", i); //start each thread (in order of input file)
	}
	
	for (i = 0; i < no_thds; i++) {
    pthread_join(id_array[i], NULL);
  }

  for (i = 0; i < length(); i++) free(deleteFirst()); // delete any remaining nodes before exiting
  
	if (read_thds != NULL) free(read_thds);
	
	pthread_mutex_destroy(&ll_mutex);
	pthread_mutex_destroy(&trans_mutex);
	pthread_cond_destroy(&turn_cond);
	
	return (0);
}

double get_current_machine_time() {
	long current_ms; // Milliseconds
	struct timespec finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &finish);

	//printf("start time %ld current time %ld \n", initial.tv_nsec, finish.tv_nsec);
	elapsed = (finish.tv_sec - initial.tv_sec);
	elapsed += (finish.tv_nsec - initial.tv_nsec) / 100000000.0;
	return elapsed;
}

void *thread_start(void *thread_ptr) {
	thd *thread_obj = (thd*)thread_ptr;

	int id = thread_obj->id;
	int arrival_time = thread_obj->arrival_time;
	int transmission_time = thread_obj->transmission_time;
	int priority = thread_obj->priority;
  int position = thread_obj->position;

  usleep(100000 * arrival_time);
  //TODO: use relative machine time
  // clock_gettime(CLOCK_REALTIME, &tm); investigate this 
  printf("Flow %2d arrives: arrival time (%.2f), transmission time (%.1f), priority (%d)\n", id, (double)get_current_machine_time(), (double)transmission_time, priority);
  
  thread_transmission(id, arrival_time, transmission_time, priority, position);
}

void thread_transmission(int id, int arrival_time, int transmission_time, int priority, int position) {

	// mutex
	// indicate node is scheduled
	// sort linked list
	// mutex
	pthread_mutex_lock(&ll_mutex);
	
	//updateScheduled(id, 1);
	insertFirst(id, arrival_time, transmission_time, priority, position);
	sort();
	
	pthread_mutex_unlock(&ll_mutex);
	
	// lock trans mutex
	pthread_mutex_lock(&trans_mutex);
	
	// wait for cond var and for having the first spot
	while (getFirstId() != id){
		printf("Flow %2d waits for the finish of flow %2d \n", id, getFirstId());
		pthread_cond_wait(&turn_cond, &trans_mutex); // release mutex(trans_mutex), wait on turn_cond, until it is signaled	
	}
	
	pthread_mutex_lock(&ll_mutex);
	
	//updateScheduled(id, 2);
	
	pthread_mutex_unlock(&ll_mutex);
	
	//TODO: use relative time
	printf("Flow %2d starts its transmission at time %.2f. \n", id, (double) get_current_machine_time());
	// perform transmission (sleep)
	usleep(100000 * transmission_time);
	//TODO: use relative time
	printf("Flow %2d finishes its transmission at time %.2f\n", id, (double) get_current_machine_time());
	
	// mutex 
	// remove node from linked list
	// mutex
	
	pthread_mutex_lock(&ll_mutex);
	free(delete(id));
	pthread_mutex_unlock(&ll_mutex);
	pthread_cond_signal(&turn_cond); // signal convar
	pthread_mutex_unlock(&trans_mutex);
}

void read_input_file() {
	char path[MAX_PATH_LEN], line[MAX_LINE_LEN], id[MAX_LINE_LEN], 
		arrival_time[MAX_LINE_LEN], transmission_time[MAX_LINE_LEN], priority[MAX_LINE_LEN];
  FILE* input_file;
  int counter = 0;

	// TODO: use actual argument
  snprintf(path, MAX_PATH_LEN, "%s", "flow.txt");

  input_file = fopen(path, "r");

  if(!input_file) { 
		perror("Could not open input file\n");
		return;
	}

  while (fgets(line, MAX_LINE_LEN, input_file)) {
  	int len = strlen(line);
  	int i = 0;
  	int position = 0;
  	int status = 0;
  	
  	if (counter == 0) {
  		no_thds = atoi(line);
  		read_thds = malloc(no_thds * sizeof(thd));
  		counter++;
  		continue;
  	}

		for (; i < len; i++) {
			if (status == 0) { // looking for thread id
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
			
			if (status == 1) { // looking for thread arrival time
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
			
			if (status == 2) { // looking for thread transmission time
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
			
			if (status == 3) { // looking for thread priority
				if (line[i] == '\n') {
					priority[position] = '\0';
					break;
				} else {
					priority[position] = line[i];
					position++;
				}
			}
		}
		
		read_thds[counter - 1].id = atoi(id);
		read_thds[counter - 1].arrival_time = atoi(arrival_time);
		read_thds[counter - 1].transmission_time = atoi(transmission_time);
		read_thds[counter - 1].priority = atoi(priority);
		read_thds[counter - 1].position = counter;
		//insertFirst(atoi(id),  atoi(arrival_time),  atoi(transmission_time), atoi(priority), counter, 0);
		counter++; // keeps track of position of thread in file
	}

  fclose(input_file);
}

