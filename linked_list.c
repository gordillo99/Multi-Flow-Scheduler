#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// in order to reduce the amount of potential bugs, linked list code based from https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm

typedef struct thread  
{
	 int id;
	 int arrival_time;
	 int transmission_time;
	 int priority;
   int position;
	 int executing;
   struct thread *next;
} thd;

struct thread *head = NULL;
struct thread *current = NULL;

void *emalloc(int n) { // custom malloc function with error checking
 	void *p;
 	p = malloc(n);
 	if (p == NULL) { // error handling
	 	perror("command malloc failed\n");
		exit(EXIT_FAILURE);
 	}
 	return p;
}

//display the list
void printList() {
   struct thread *ptr = head;
   //start from the beginning
   while(ptr != NULL) {
      printf("(id: %d, arrival_time: %d, transmission_time: %d, priority: %d, original position: %d, executing: %d)\n",
      	ptr->id, ptr->arrival_time, ptr->transmission_time, ptr->priority, ptr->position, ptr->executing);
      ptr = ptr->next;
   }
   printf("\n");
}

int getFirstId() {
	if (head == NULL) return -1;
	else return head->id;
}

//insert link at the first location
void insertFirst(int id, int arrival_time, int transmission_time, int priority, int position)
{
   //create a link
   struct thread *link = (struct thread*) emalloc(sizeof(struct thread));
	
   link->id = id;
   link->arrival_time = arrival_time;
   link->transmission_time = transmission_time;
   link->priority = priority;
   link->position = position;
	 link->executing = 0;
	
   //point it to old first thread
   link->next = head;
	
   //point first to new first thread
   head = link;
}

//delete first item
struct thread* deleteFirst()
{

   //save reference to first link
   struct thread *tempLink = head;
	
   //mark next to first link as first 
   head = head->next;
	
   //return the deleted link
   return tempLink;
}

//is list empty
int isEmpty()
{
   return head == NULL;
}

int length()
{
   int length = 0;
   struct thread *current;
	
   for(current = head; current != NULL; current = current->next)
	{
      length++;
   }
	
   return length;
}

//find a link with given key
struct thread* find(int position){

   //start from the first link
   struct thread* current = head;

   //if list is empty
   if(head == NULL)
	{
      return NULL;
   }

   //navigate through list
   while(current->position != position){
	
      //if it is last thread
      if(current->next == NULL){
         return NULL;
      }else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}

//delete a link with given id
struct thread* delete(int id){

   //start from the first link
   struct thread* current = head;
   struct thread* previous = NULL;
	
   //if list is empty
   if(head == NULL){
      return NULL;
   }

   //navigate through list
   while(current->id != id){
	
      //if it is last thread
      if(current->next == NULL){
         return NULL;
      }else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;             
      }
		
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   }else {
      //bypass the current link
      previous->next = current->next;
   }    
	
   return current;
}

//delete a link with given id
struct thread* deletePosition(int position){

   //start from the first link
   struct thread* current = head;
   struct thread* previous = NULL;
	
   //if list is empty
   if(head == NULL){
      return NULL;
   }

   //navigate through list
   while(current->position != position){
	
      //if it is last thread
      if(current->next == NULL){
         return NULL;
      }else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;             
      }
		
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   }else {
      //bypass the current link
      previous->next = current->next;
   }    
	
   return current;
}

void updateExecuting(int id, int executing){

   //start from the first link
   struct thread* current = head;

   //if list is empty
   if(head == NULL)
	 {
      return;
   }

   //navigate through list
   while(current->id != id){
	
      //if it is last thread
      if(current->next == NULL){
         return;
      }else {
         //go to next link
         current = current->next;
      }
   }      
	
   current->executing = executing;
}

void sort() {

   int i, j, k, temp_id, temp_arrival_time, temp_transmission_time, temp_priority, temp_position, temp_executing;
   struct thread *current;
   struct thread *next;
	
   int size = length();
   k = size ;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = head;
      next = head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   
				 int switch_flag = 0;
				 
         if (current->executing < next->executing) {
				   switch_flag = 1;
				 } else if (current-> executing == next->executing) {
				   if (current->priority > next->priority) {
		         switch_flag = 1;
		       } else if (current->priority == next->priority) {
		       	 if (current->arrival_time > next->arrival_time) {
		       	   switch_flag = 1;
		       	 } else if (current->arrival_time == next->arrival_time) {
		       	   if (current->transmission_time > next->transmission_time) {
				         switch_flag = 1;
				       } else if (current->transmission_time == next->transmission_time) {
				         if (current->position > next->position) {
				           switch_flag = 1;
				         }
				       }
		       	 }
		       }
				 }	 
         
         if (switch_flag) {
            temp_id = current->id;
						current->id = next->id;
						next->id = temp_id;

						temp_arrival_time = current->arrival_time;
						current->arrival_time = next->arrival_time;
						next->arrival_time = temp_arrival_time;

						temp_transmission_time = current->transmission_time;
						current->transmission_time = next->transmission_time;
						next->transmission_time = temp_transmission_time;

						temp_priority = current->priority;
						current->priority = next->priority;
						next->priority = temp_priority;

						temp_position = current->position;
						current->position = next->position;
						next->position = temp_position;

						temp_executing = current->executing;
						current->executing = next->executing;
						next->executing = temp_executing;
         }
			
         current = current->next;
         next = next->next;
      }
   }
}


