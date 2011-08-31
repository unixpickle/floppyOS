#include <kernel/kstdlib.h>

#define kTaskListBase (task_t *)0x20000
#define kTaskCount (int *)0x509
#define kTaskCurrent (int *)0x50D
#define kTaskPID (pid_t *)0x511

#define kTaskSpaceStart (void *)0x100000
#define kTaskSpacePerTask 0x10000
#define kTaskSpacePerKernTask 0x10000

typedef unsigned int pid_t;

typedef struct {
	char ldt[16];
	pid_t pid;
	unsigned int edi;
	unsigned int esi;
	unsigned int ebp;
	unsigned int esp;
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;
	unsigned int eax;
	unsigned short ds;
	unsigned short ss;
	unsigned short gs;
	unsigned short fs;
	unsigned short es;
	unsigned short cs;
	unsigned int eip;
	unsigned int eflags;
	unsigned int waitingLock;
	char * basePtr;
} __attribute__((__packed__)) task_t;


void task_list_reset ();
int task_list_count ();
pid_t task_list_current ();
osStatus task_get (pid_t pid, task_t * destination);
pid_t task_next_pid ();
osStatus task_kill (pid_t pid);
osStatus task_start (char * codeBase, unsigned short length, pid_t * pid);
void * task_translate_addr (void * existing);

task_t * task_config (void * gdtBase);

