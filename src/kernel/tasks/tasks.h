#include <kernel/kstdlib.h>

#define kTaskListBase (task_t *)0x200000
#define kTaskCount (int *)0x509
#define kTaskCurrent (int *)0x50D
#define kTaskPID (pid_t *)0x511

#define kTaskSpaceStart (void *)0x300000
#define kTaskSpacePerTask 0x10000
#define kTaskSpacePerKernTask 0x10000

typedef unsigned int pid_t;

typedef struct {
	char ldt[16];
	pid_t pid;
	unsigned int esp;
	char * basePtr;
	unsigned char hasStarted; // 0 = NO, 1 = YES
	unsigned char reserved;
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

