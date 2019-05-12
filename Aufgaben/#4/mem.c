/*	Speicherverwaltung

	Unsere Datenstruktur ist LinkList 

	o----o----o----o----o----o----NULL

	Wobei jedes o ein Knoten (Datum) ist. 
	Knoten speichert: key (position), wert (string), *next (nächster Knoten)

	String wird als char* gespeichert.

*/

// INCLUDES
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// DEFINES
#define MEMSIZE 256	

// STRUCTS
struct node{
	int value;
	// char memory[MEMSIZE];
	struct node *pred;
	struct node *succ;
};

struct linkedList{			// Funktioniert wegen pop() nur fuer positive Zahlen. 
	int amount;				// ist aber ausreichend für unseren Speicher. Weil alle Zellen
	struct node *tail;		// positive Nummer haben
};


// FUNKTIONEN
void memory_init();
	
	// fuer Linked-List
void push(struct linkedList *list, int x);
int pop(struct linkedList *list);
void displayTailtoHead(struct linkedList list);



// MAIN
int main(int argc, char *argv[]){
	struct linkedList list = {0, NULL};
	int input;

	// Liste fuellen
	do{
		// Eingabe
		printf("Zahl: ");
		scanf("%d", &input);

		// Liste fuellen
		if(input != -1) push(&list, input);
	}while(input != -1);

	// Liste ausgeben
	displayTailtoHead(list);

	// Liste leeren
	while(list.tail != NULL){
		printf("%d\n", list.amount);
		pop(&list);
	}
	return(0);
}



void push(struct linkedList *list, int x){

	// leere Liste
	if(list->amount == 0){
		// neuer Speicherplatz für tail, muss Speicher für Node haben
		list->tail = malloc(sizeof(struct node));	
		struct node tmp = {x, NULL, NULL};
		*list->tail = tmp;
		list->amount++;
		return;
	}

	// nicht leere Liste
	list->tail->succ = malloc(sizeof(struct node));	

	struct node tmp = {x, list->tail, NULL};
	*list->tail->succ = tmp;
	list->tail = list->tail->succ;			// Pointer nur veraendern, nicht Inhalt
	list->amount++;
	return;	
}

int pop(struct linkedList *list){
	// Liste leer
	if (list->amount == 0){
		return -1;
	}

	// Liste nur ein Element
	if(list->amount == 1){
		int res = list->tail->value;
		free(list->tail);
		list->tail = NULL;
		list->amount--;
		return res;
	}
	// liste nicht leer
	int res = list->tail->value;

	list->tail = list->tail->pred;
	free(list->tail->succ);
	list->amount--;

	return res;
}

void displayTailtoHead(struct linkedList list){

	// Leere Liste
	if(list.amount == 0){
		printf("[]\n");
		return;
	}

	struct node *start = list.tail;		// Merke dir den Pointer

	printf("[");
	while(list.tail != NULL){
		if(list.tail == start) 	printf("%d", list.tail->value);	// Ist das der End/Startknoten?
		else					printf(", %d", list.tail->value);
		list.tail = list.tail->pred;
	}
	printf("]\n");

	return;
}