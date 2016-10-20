struct thread;
void *emalloc(int n);
void insertFirst(int key, int id, int arrival_time, int transmission_time, int priority, int position);
struct thread* deleteFirst();
int isEmpty();
int length();
struct thread* find(int id);
struct thread* delete(int id);



