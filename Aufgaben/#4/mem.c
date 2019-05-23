// INCLUDES
#include <stdio.h>
#include <string.h> //strcpy

// DEFINEs
#define MEM_SIZE 256

//STRUCTS
struct mem_block{
	size_t size;
	struct mem_block *next;
};

//FUNKTIONEN
void memory_init();
void* memory_allocate(size_t byte_count);
void memory_free(void* pointer);
void memory_print();

//VARIABLEN
char memory[MEM_SIZE];
struct mem_block *first;

int main(){
	//Linked List initialisieren:
	memory_init();
	
	//Speicher reservieren
	char *h = (char *) memory_allocate(6), *w = (char *) memory_allocate(5), *ptr = (char *) memory_allocate(31), *end = (char *) memory_allocate(14);
	strcpy(h, "Hello");
	strcpy(w, "Welt");
	strcpy(ptr, "Dieser Text liegt im Speicher.");
	strcpy(end, "Letzter Block");

	//Speicher ausgeben
	puts("Gesamter Text:");
	memory_print();

	//Block entfernen und durch zwei andere ersetzen
	memory_free((void *) ptr);
	char *ptra = (char *) memory_allocate(6), *ptrb = (char *) memory_allocate(5);
	strcpy(ptra, "Neuer");
	strcpy(ptrb, "Text");
	puts("\nBlock löschen und durch zwei andere ersetzen:");
	memory_print();
	
	//Letztes Element löschen
	memory_free((void *) end);
	puts("\nLetztes Element entfernt:");
	memory_print();

	return 0;
}

void memory_init(){
	first = (struct mem_block*) &memory;//Speicher der Größe MEM_SIZE suchen
	first->size = 0;//size=0 entspricht leeren Speicher und kann also für die erste Variable genutzt werden
	first->next = NULL;
}

void* memory_allocate(size_t byte_count){
	struct mem_block *zs = first;//um first wieder auf dem Anfangszustand zu setzen
	unsigned long last = (unsigned long)(first + 1) + first->size; //letzte noch beschriebene Position
	while (first->next != NULL){
		if (first->size == 0 && (size_t)((unsigned long)(first->next)) - (size_t)((unsigned long)(first + 1)) >= byte_count){ //falls leerer Block vorhanden und Lücke groß genug ist
			first->size = byte_count;
			struct mem_block *ret = first;
			first = zs;
			return (void *) (ret + 1);//Speicherposition gefunden
		}else if ((unsigned long)first->next - ((unsigned long)(first + 1) + first->size) >= byte_count + sizeof(struct mem_block)){ //guckt ob Lücke groß genug ist
			struct mem_block *new_block = (struct mem_block *) ((unsigned long)(first + 1) + first->size);//neues Element der Liste(Speciherblock)
			new_block->size = byte_count;
			new_block->next = first->next;
			first->next = new_block;
			first = zs;
			return (void *) (new_block + 1);//Speicherposition gefunden
		}
		last = (unsigned long)(first + 1) + first->size; //letzte noch beschriebene Position aktuallisieren
		first = first->next;
	}

	//Es wurde kein freier Platz gefunden, also wird probiert hinten ein Block anzuhängen
	if (first->size == 0 && (size_t)((unsigned long)(first->next)) - (size_t)((unsigned long)(first + 1)) >= byte_count){ //falls leerer Block (size = 0) vorhanden und Lücke danach groß genug ist
		first->size = byte_count;
		struct mem_block *ret = first;
		first = zs;
		return (void *) (ret + 1);//Speicherposition gefunden
	}else if ((unsigned long) MEM_SIZE - ((unsigned long)first - (unsigned long)zs) >= byte_count + sizeof(struct mem_block)){ //test ob am Ende noch Platz ist
		struct mem_block *new_block = (struct mem_block *) ((unsigned long)(first + 1) + first->size);
		new_block->size = byte_count;
		new_block->next = first->next;
		first->next = new_block;
		first = zs;
		return (void *) (new_block + 1);//Speicherposition gefunden
	}else{ //keine Position gefunden
		first = zs;
		return (void *)-1;
	}
}

void memory_free(void* pointer){
	struct mem_block *zs = first; // dient als Zwischenspeicher um die Linked List anzupassen
	struct mem_block *block = (struct mem_block *) pointer; // Block der entfern werden soll
	block -= 1;
	if (block == first){//falls erster Speicherblock gelöscht werden soll
		first->size = 0;//Speicher jz leer
		char *c = (char *) (first + 1);
		*c = '\0';//String clearn
	}
	struct mem_block *prev = zs;
	while (zs->next != NULL && zs != block){//läuft eig. bis zs die Speicherzelle vor dem zu löschenden Block ist
		prev = zs;
		zs = zs->next;
	}
	if (zs->next != NULL){//sonst error, OutOfBounds
		prev->next = zs->next;// Zu löschendes Element aus Liste entfernen
	}else{ //ist letztes Element der Linked List und LinkedList hat mind. zwei Elemente (zs hat vorgänger)
		prev->next = NULL;
	}
}

void memory_print(){
	struct mem_block *zs = first;//um first wieder auf dem Anfangszustand zu setzen
	int mem_block_num = 1; // Beinhaltet um welchen Block es sich handelt (für die Ausgabe)
	while (first != NULL){
		printf("---Block: %d:---Adr.: %d-\n\t", mem_block_num, first);
		mem_block_num++;
		char *c = (char *) (first + 1);//beinhaltet Speicheradresse des ersten Char des Speicherblocks 
		for (size_t i = 0; i < first->size; i++)
			printf("%c", *(c++)); //gibt char aus und schiebt Pointer um eine Stelle weiter
		puts("");//new line
		first = first->next;
	}
	printf("---End of memory---\n\n");
	first = zs;
}