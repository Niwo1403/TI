#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <inttypes.h>

struct Process {
	uint64_t pId ;
	uint64_t cycles_done;	
	uint64_t cycles_waited;
	uint64_t cycles_todo;
	
	struct Process* next;
	struct Process* prev;
};

struct Process* rr(struct Process* head, struct Process* current);
struct Process* fcfs(struct Process* head, struct Process* current);
struct Process* spn(struct Process* head, struct Process* current);
struct Process* srt(struct Process* head, struct Process* current);
struct Process* hrrnPreemptive(struct Process* head, struct Process* current);
struct Process* hrrnNonPreemptive(struct Process* head, struct Process* current);

#endif
