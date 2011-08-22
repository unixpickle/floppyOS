#include <kernel/kstdlib.h>

#define kTaskListBase (task_t *)0x20000
#define kTaskCount (int *)0x509
#define kTaskCurrent (int *)0x50D
#define kTaskPID (pid_t *)0x511

#define kTaskSpaceStart (void *)0x30000

typedef unsigned int pid_t;

typedef struct {
	char ldt[16];
	pid_t pid;
	unsigned int ebp;
	unsigned int esp;
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int esi;
	unsigned int edi;
	unsigned int ss;
	unsigned int ds;
	unsigned int gs;
	unsigned int fs;
	unsigned int es;
	unsigned int cs;
	char * basePtr;
} task_t;


void task_list_reset ();
int task_list_count ();
pid_t task_list_current ();
osStatus task_get (pid_t pid, task_t * destination);
pid_t task_next_pid ();
osStatus task_kill (pid_t pid);
osStatus task_start (char * codeBase, unsigned short length, pid_t * pid);

