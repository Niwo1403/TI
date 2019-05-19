#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "scheduler.h"

#define NUM_PROCESSES	4
#define SCHED_NUM		6

#define RED		"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define MAGENTA	"\033[35m"
#define CYAN	"\033[36m"
#define NORMAL	"\033[39m"

char* algos[] = {
	"Round Robin",
	"First Come First Serve",
	"Shortest Process Next",
	"Shortest Remain Time Next",
	"Highest Response Ratio Next (preemptive)",
	"Highest Response Ratio Next (non-preemptive)"
};

struct Process* updateProcesses(struct Process* head, struct Process* current, uint64_t processes[][2], size_t dim_1, uint64_t step) {
	// Listen einträge updaten
	struct Process* cursor = head->next;
	struct Process* ret_val = NULL;
	while(cursor != head) {
		if(cursor == current) {
			cursor->cycles_done++;
			cursor->cycles_todo--;
		} else {
			cursor->cycles_waited++;
		}
		
		struct Process* tmp = cursor;
		cursor = cursor->next;
		// fertige Prozesse löschen
		if(!tmp->cycles_todo) {
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			ret_val = tmp;
		}
	}
	
	// neue Prozesse einfügen
	for(size_t i=0; i<dim_1; i++) {
		if(processes[i][0] == step) {
			struct Process* new_process = malloc(sizeof(struct Process));
			new_process->pId = i;
			new_process->cycles_done = 0;
			new_process->cycles_todo = processes[i][1];
			new_process->cycles_waited = 0;
			new_process->next = head;
			new_process->prev = head->prev;
			
			head->prev->next = new_process;
			head->prev = new_process;
		}
	}
	
	return ret_val;
}

static inline void printHead(uint8_t algo) {
	printf(YELLOW "%s\n" NORMAL, algos[algo]);
	fputs("+-------+-------+---------------+-------+\n",stdout);
	fputs(
		"|" CYAN "time\t" NORMAL
		"|" CYAN "run\t" NORMAL
		"|" CYAN "wait\t\t" NORMAL
		"|" CYAN "done\t" NORMAL
		"|\n",
		stdout
	);
	fputs("+-------+-------+---------------+-------+\n",stdout);
}

void printProcesses(uint64_t step, struct Process* head, struct Process* current, struct Process* kill) {	
	char waiting_line[16];
	memset(waiting_line, 0, 16);
	struct Process* cursor = head->next;
	while(cursor != head) {
		if(cursor != current) {
			char tmp[5];
			snprintf(tmp, 5, "%"PRIu64",", cursor->pId);
			strncat(waiting_line, tmp, 16);
		}
		
		cursor = cursor->next;
	}
	waiting_line[strlen(waiting_line)-1] = 0;
	if(strlen(waiting_line) >= 7) {
		strncat(waiting_line, "\t", 16);
	} else {
		strncat(waiting_line, "\t\t", 16);
	}
	
	char done_str[12];
	if(kill) {
		snprintf(done_str, 12, "%"PRIu64"\t", kill->pId);
	} else {
		snprintf(done_str, 12, "\t");
	}
	char curr_str[12];
	if(current) {
		snprintf(curr_str, 12, "%"PRIu64"\t", current->pId);
	} else {
		snprintf(curr_str, 12, "\t");
	}
	
	printf(
		"|" CYAN "%"PRIu64"\t" NORMAL "|" BLUE "%s" NORMAL "|" RED "%s" NORMAL "|" GREEN "%s" NORMAL "|\n", 
		step,
		curr_str,
		waiting_line,
		done_str
	);
}

int main() {
	uint64_t processes[NUM_PROCESSES][2] = {{0, 3}, {2, 7}, {4, 1}, {6, 5}};
	struct Process* (*schedul[SCHED_NUM])(struct Process*, struct Process*) = {
		rr,
		fcfs,
		spn,
		srt,
		hrrnPreemptive,
		hrrnNonPreemptive
	};

	char buffer[10];
	for(uint8_t i=0; i<SCHED_NUM; i++) {
		struct Process* head = malloc(sizeof(struct Process));
		head->pId = 0;
		head->cycles_done = head->cycles_todo = head->cycles_waited = 0;
		head->next = head->prev = head;
		
		struct Process* curr = NULL;
		uint64_t step = 0;
		printHead(i);
		do {
			// gibt fertigen Prozess zurück
			struct Process* to_kill = updateProcesses(head, curr, processes, NUM_PROCESSES, step);
			curr = schedul[i](head, curr);
			printProcesses(step, head, curr, to_kill);
			free(to_kill);
			step++;
		} while(curr);
		fputs("+-------+-------+---------------+-------+\n",stdout);
		free(head);
		fputs("Press Enter for next schedulig-algorithm.\n\n", stdout);
		fgets(buffer, 10, stdin);
	} 
	
	return EXIT_SUCCESS;
}

