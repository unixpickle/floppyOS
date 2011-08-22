#include "tasks.h"

void task_list_reset () {
	int * countBuf = kTaskCount;
	int * currBuf = kTaskCurrent;
	pid_t * pid = kTaskPID;
	*countBuf = 0;
	*currBuf = -1;
	*pid = 0;
}

int task_list_count () {
	int * countBuf = kTaskCount;
	int c = *countBuf;
	return c;
}

pid_t task_list_current () {
	int * currBuf = kTaskCurrent;
	if (*currBuf == -1) return 0;
	unsigned int offset = sizeof(task_t) * (*currBuf);
	task_t * task = (task_t *)((unsigned int)(kTaskListBase) + offset);
	return task->pid;
}

osStatus task_get (pid_t pid, task_t * destination) {
	int * countBuf = kTaskCount;
	int i;
	for (i = 0; i < *countBuf; i++) {
		unsigned int offset = sizeof(task_t) * i;
		task_t * task = (task_t *)((unsigned int)(kTaskListBase) + offset);
		if (task->pid == pid) {
			kmemcpy((char *)destination, (char *)task, sizeof(task_t));
			return osOK;
		}
	}
	return osGeneralError;
}

pid_t task_next_pid () {
	int * pidBuf = kTaskPID;
	*pidBuf += 1;
	return *pidBuf;
}

osStatus task_kill (pid_t pid) {
	// remove this from the task list.
	int * countBuf = kTaskCount;
	int i, numGone = 0;
	task_t * nextTask = kTaskListBase;
	task_t * dstTask = kTaskListBase;
	for (i = 0; i < *countBuf; i++) {
		if (nextTask->pid == pid) {
			numGone += 1;
		} else {
			kmemcpy((char *)dstTask, (char *)nextTask, sizeof(task_t));
			dstTask += sizeof(task_t);
		}
		nextTask += sizeof(task_t);
	}
	*countBuf -= numGone;
	if (numGone == 1) return osOK;
	return osGeneralError;
}

osStatus task_start (char * codeBase, unsigned short length, pid_t * pid) {
	// find the next available program space
	void * baseAddr = kTaskSpaceStart;
	int * countBuf = kTaskCount;
	int i;
	bool isGood = true;
	while (true) {
		// check if it's good
		for (i = 0; i < *countBuf; i++) {
			unsigned int offset = sizeof(task_t) * i;
			task_t * task = (task_t *)((unsigned int)kTaskListBase + offset);
			if (task->basePtr == baseAddr) {
				isGood = false;
				break;
			}
		}
		if (isGood) {
			break;
		} else baseAddr += 0x10000;
	}
	// create our task
	task_t * ourTask = *countBuf * sizeof(task_t) + kTaskListBase;
	for (i = 0; i < sizeof(task_t); i++) {
		((char *)ourTask)[i] = 0;
	}
	ourTask->basePtr = baseAddr;
	return osOK;
}

