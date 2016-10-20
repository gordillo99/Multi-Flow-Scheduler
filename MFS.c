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

int main (int argc, char **argv) {

	read_input_file();

	return (0);
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
		perror("Could not open input file.\n");
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
		printf("id: %s\n", id);
		printf("arrival: %s\n", arrival_time);
		printf("transmission: %s \n", transmission_time);
		printf("priority: %s\n", priority);
		printf("position: %d\n", counter);
		counter++; // keeps track of position of thread in file
	}
  printf("%d\n", no_threads);

  fclose(input_file);
}

