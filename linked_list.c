#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// in order to reduce the amount of potential bugs, linked list code taken from https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm

struct thread  
{
	 int id;
	 int arrival_time;
	 int transmission_time;
	 int priority;
   int position; 
   struct thread *next;
};

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

//insert link at the first location
void insertFirst(int key, int id, int arrival_time, int transmission_time, int priority, int position)
{
   //create a link
   struct thread *link = (struct thread*) emalloc(sizeof(struct thread));
	
   link->id = id;
   link->arrival_time = arrival_time;
   link->transmission_time = transmission_time;
   link->priority = priority;
   link->position = position;
	
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
struct thread* find(int id){

   //start from the first link
   struct thread* current = head;

   //if list is empty
   if(head == NULL)
	{
      return NULL;
   }

   //navigate through list
   while(current->id != id){
	
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

//delete a link with given key
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


