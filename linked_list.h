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
void updateExecuting(int id, int executing);
struct thread* deleteFirst();
int isEmpty();
int length();
struct thread* find(int position);
struct thread* delete(int id);
struct thread* deletePosition(int position);
void updateScheduled(int position, int scheduled);
void sort();
int getFirstId();
void printList();



