typedef struct thread  
{
	 int id;
	 int arrival_time;
	 int transmission_time;
	 int priority;
   int position; 
   struct thread *next;
} thd;
void *emalloc(int n);
void insertFirst(int id, int arrival_time, int transmission_time, int priority, int position);
struct thread* deleteFirst();
int isEmpty();
int length();
struct thread* delete(int id);
void sort();
int getFirstId();
void printList();



