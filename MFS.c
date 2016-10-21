#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "linked_list.h"

#define MAX_LINE_LEN 256
#define MAX_PATH_LEN 40

void read_input_file();
void *thread_start(void *thread_ptr);

pthread_t * thread_ids;
pthread_mutex_t ll_mutex; // read-write linked list mutex declaration

int main (int argc, char **argv) {
	int number_of_threads;
	int i;
	
	read_input_file();
	number_of_threads = length();
	pthread_t id_array[number_of_threads];
	thread_ids = id_array;
	for (i = 0; i < number_of_threads; i++) {
		if(pthread_create(&id_array[i], NULL, thread_start, (void *) find(i+1)) != 0) printf("Can't create thread %d\n", i); //start each thread (in order of input file)
	}
	
	
	
	for (i = 0; i < number_of_threads; i++) {
    pthread_join(id_array[i], NULL);
  }

  for (i = 0; i < length(); i++) free(deleteFirst()); // delete any remaining nodes before exiting

	return (0);
}

void *thread_start(void *thread_ptr) {
	thd *thread_obj = (thd*)thread_ptr;

	int id = thread_obj->id;
	int arrival_time = thread_obj->arrival_time;
	int transmission_time = thread_obj->transmission_time;
	int priority = thread_obj->priority;
  int position = thread_obj->position;

  usleep(100000 * arrival_time);
  // clock_gettime(CLOCK_REALTIME, &tm); investigate this 
  
	printf("inside thread %d \n", thread_obj->position);
}

void thread_transmission(int id, int transmission_time) {

	// mutex
	// indicate node is scheduled
	// sort linked list
	// mutex
	pthread_mutex_lock(&ll_mutex);
	
	updateScheduled(id, 1);
	sort();
	
	pthread_mutex_unlock(&ll_mutex);
	
	// wait for cond var and for having the first spot
	
	// perform transmission (sleep)
	
	
	
	// mutex 
	// remove node from linked list
	// mutex
	/*
	pthread_mutex_lock(&ll_mutex);
	
	
	
	pthread_mutex_unlock(&ll_mutex);
	*/
}

void read_input_file() {
	char path[MAX_PATH_LEN], line[MAX_LINE_LEN], id[MAX_LINE_LEN], 
		arrival_time[MAX_LINE_LEN], transmission_time[MAX_LINE_LEN], priority[MAX_LINE_LEN];
  FILE* input_file;
  int counter = 0;
  int no_threads = 0;

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
  		no_threads = atoi(line);
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

		insertFirst(atoi(id),  atoi(arrival_time),  atoi(transmission_time), atoi(priority), counter, 0);
		counter++; // keeps track of position of thread in file
	}

  fclose(input_file);
}

