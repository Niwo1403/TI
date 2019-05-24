// Nicolai Wolfrom und Joel Heuer
// gcc -std=c11 -Wall -Wextra -pedantic sched_test.c scheduler.c -o prog

#include <stdlib.h>
#include <unistd.h>	// SLEEP
#include <stdio.h> // DEBUGGINNG
#include "scheduler.h"


// FAIL
struct Process* rr(struct Process* head, struct Process* current) {

	// fuer die erste Iteration
	if(current == NULL) {
		return head->next;
	}

	struct Process *process = current->next;

	if(process == head) process = process->next;
	
	// solange bis alle jmd was zu tun hat
	while(process->cycles_todo == 0) {
		// wenn alles abgearbeitet wurde, return NULL
		if(process -> pId == process -> prev -> pId)
			return NULL;
		
		process = process->next;		
	}
	return process;
}

struct Process* fcfs(struct Process* head, struct Process* current) {
	// checken, ob aktueller Prozess noch was zu tun hat
	// --> damit keine Warnung
	if(current != NULL && current->cycles_todo > 0)
		return current;

	// nehme den ersten Prozess
	struct Process *nextProcess = head->next;

	// checken, ob Prozess noch was zu tun hat
	// und falls nicht, dann sein Nachfolger
	while(nextProcess->cycles_todo == 0){
		// falls man wieder am head ist, wurden alle schon abgearbeitet
		if(nextProcess->pId == head->pId) return(NULL);
		// sonst den Nachfolger nehmen
		nextProcess = nextProcess->next;
	}

	// es hat noch ein Prozess etwas zu tun
	return(nextProcess);
}

struct Process* spn(struct Process* head, struct Process* current) {
	if(current != NULL){	// damit keine Warnung kommt
		;
	}

	// nehme den ersten Prozess
	struct Process *currentProcess = head->next;
	struct Process *shortest = head->next;	

	// Ermitteln, wer der kürzste Prozess ist, solange
	// bis wieder am Start angekommen
	do{
		// falls kürzer Prozess existiert
		if(shortest->cycles_todo > currentProcess->cycles_todo 
			&& currentProcess->cycles_todo > 0)
			shortest = currentProcess;
		// naechsten Prozess nehmen, bis head erreicht
		currentProcess = currentProcess->next;
	}while(currentProcess->pId != head->pId);

	// Falls alle Prozesse schon abgearbeitet sind.
	if(shortest->cycles_todo == 0) 
		return(NULL);
	return(shortest);
}

struct Process* srt(struct Process* head, struct Process* current) {
	// keine Warnung
	struct Process *process;
	process = head;
	process = current;
	return process;
}


// hier muss man aufpassen, dass man nicht durch 0 teilt, wenn
// cycles_todo == 0 ist.
struct Process* hrrnPreemptive(struct Process* head, struct Process* current) {
		if(current != NULL){	// damit keine Warnung kommt
			;
		}

	int endFlag = 0;	// Wenn alle Prozesse nix zu tun haben, dann ist das 0
	struct Process *currentProcess = head->next;
	struct Process *nextProcess = head->next;

	// checke ob zu ende
	if(currentProcess->cycles_todo == 0) return(NULL);

	double nextRatio = (nextProcess->cycles_waited + nextProcess->cycles_todo)
									/ nextProcess->cycles_todo;

	// solange bis wieder beim Kopf der LinkedList
	do{
		if(currentProcess->cycles_todo != 0) endFlag++;
		// Falls 0, dann nehme pruefe naechsten Prozess
		else{
			currentProcess = currentProcess->next;
			continue;
		}

		double currentRatio = (currentProcess->cycles_waited + currentProcess->cycles_todo)
									/ currentProcess->cycles_todo;
		// Falls hoehere Ratio, dann soll der Prozess ausgeführt werden
		if(currentRatio > nextRatio){
			nextProcess = currentProcess;
			nextRatio = currentRatio;
		}

		currentProcess = currentProcess->next;
	}while(currentProcess->pId != head->pId);

	// Falls alle abgearbeitet wurden, NULL
	if(endFlag == 0) return(NULL);

	return(nextProcess);

}

struct Process* hrrnNonPreemptive(struct Process* head, struct Process* current) {
	// keine warnung
	struct Process *process;
	process = head;
	process = current;
	return process;
}

