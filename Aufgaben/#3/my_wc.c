#include <stdio.h>

#define SIZE 256	// Array groesse

int wc(char *);
int my_isspace(char x);
int my_isword(int *, char);

int my_strlen(char *);

int main(int argc, char *argv[]){

	// Falls Pipe-Eingabe
	if(argc < 2){ 
		char buffer[SIZE];
		int  len;
		// zu wenige argumente, also mit stdin abfangen
		// Kopiere Inhalt von stdin in Array
		fgets(buffer, SIZE, stdin);

		// Ermittle groesse von stdin
		for(int i = 0; ! my_isspace(buffer[i]); ++i){
			len++;
			if(my_isspace(buffer[i+1])) len++;	// für '\0'
		}
		//kopiere array
		char newbuffer[len];
		for(int i = 0; i < len; ++i){
			if(i == len-1){
			 	newbuffer[i] = '\0';	// statt '\n' jetzt '\0'
			 	break;
			}
			newbuffer[i] = buffer[i];
		}
		wc(newbuffer);	
		return(0);
	}
	else wc(argv[1]);
	return(0);
}

int wc(char *filename){
	int c;					// speichert ascii wert?????????
	int wasword = 0;		// ist vorgaenger zeichen ein teil eines worts?
	int counter_word = 0;
	int counter_lines = 0;
	int counter_char  = 0;
	FILE *filepointer = fopen(filename, "r"); // Öffne erstes Argument (Muss Datei sein)
	if(filepointer == NULL){
		fprintf(stderr, "ERROR: Can't open file %s\n", filename);
		return(1);
	}

	if(filepointer == NULL){
		// Lesen moeglich
		return (1);
	}

	fprintf(stdout, "Lesen möglich.\n");

	while(  c =fgetc(filepointer), c != EOF){		// Bis EndOfFile lesen
		if(my_isword(&wasword, c)){
			counter_word++;	// isspace schaut, ob ein Leerzeichen, Zeilenumbruch da ist
		} 
		if(c == '\n') counter_lines++;
		counter_char++;
	}

	fclose(filepointer);	// Schließt Datei
	fprintf(stdout, "%d, %d, %d\n", counter_lines, counter_word, counter_char);
	return(0);
}

int my_isspace(char x){
	return( x == 32 || x == '\t' || x == '\n' || 
	    	x == '\f'  || x == '\r' || x == '\v');
}

int my_isword(int *wasword, char c){
	// FALL 1
	if(my_isspace(c)){
		*wasword = 0;
		return(0);	
	}
	// FALL 2
	else if(*wasword == 1){ // fall 2
		*wasword = 1;	
		return(0);
		printf("%c\n", c);
	}
	// FALL 3
    *wasword = 1;
	return(1);
}

// FALL 1: SPACE-Zeichen (kein Neues Wort)
// FALL 2: KEIN SPACE & man ist schon in einem Wort,
		   				// dh. Vorgaenger ist KEIN SPACE (KEIN NEUES WORT)
// FALL 3: KEIN SPACE & Vorganger war kein SPACE (NEUES WORT))

// Terminal-Eingabe
// ./prog  datei.txt liest die Datei ein
// ./prog <datei.txt liest den INHALT der Datei ein
