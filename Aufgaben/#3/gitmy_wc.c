#include <stdio.h>
#include <sys/types.h>	// pid_t
#include <sys/wait.h>	// wait(int *)
#include <unistd.h>		// fork()
#include <stdlib.h>		// exit(int)
#define SIZE 256	// Array groesse

// int nums[3] = {0,0,0};

int wc(FILE *);
int my_isspace(char x);
int my_isword(int *, char);

int my_strlen(char *);

int main(int argc, char *argv[]){

	int check = 0;		// Flag ob my_wc funktioniert hat

	// Falls Pipe-Eingabe
	if(argc < 2){
		check = wc(stdin); 
		printf("\n");
	}
	 
	else if(argc == 2){
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL){
			fprintf(stderr, "my_wc-ERROR: Can't open file %s!\n", argv[1]);// Kann Datei nicht öffnen
			return(1);
		}else{
			check = wc(fp);
			fclose(fp);
			printf("%s\n", argv[1]);
		}
	} 
	else{ /*argc > 2 */
		pid_t pids[argc-1];
		int status[argc-1]; 	// für wait();
		FILE *fp ;

		// erzeuge neue KIndprozesse mit for-loop
		for(int i = 0; i < argc-1; ++i){

			// Neuer Kindprozess
			pids[i] = fork();
			if(pids[i] == 0){
				fp = fopen(argv[i+1], "r");
				if (fp == NULL){
					fprintf(stderr, "my_wc-ERROR: Can't open file %s!\n", argv[i+1]);// Kann Datei nicht öffnen
					return(1);
				}else{
					check = wc(fp);
					printf("%s\n", argv[i+1]);
					if(check){
						fprintf(stderr, "ERROR: child-process %d\n", i);
					} 
					// Kindprozess i beenden
					fclose(fp);
				}
				exit(1);
			}
			// Elternprozess wartet bis Kindprozesse der Reihe nach beendet sind
			wait(&status[i]);
		} // for
		// printf(" %d %d %d insgesamt\n", nums[0], nums[1], nums[2]);
	}

	return(check);
}

int wc(FILE *filepointer){
	int c;					// speichert ascii wert?????????
	int wasword = 0;		// ist vorgaenger zeichen ein teil eines worts?
	int counter_word = 0;
	int counter_lines = 0;
	int counter_char  = 0;

	while(  c =fgetc(filepointer), c != EOF){		// Bis EndOfFile lesen
		if(my_isword(&wasword, c)){
			counter_word++;	// isspace schaut, ob ein Leerzeichen, Zeilenumbruch da ist
		} 
		if(c == '\n') counter_lines++;
		counter_char++;
	}
	fprintf(stdout, "%d %d %d ", counter_lines, counter_word, counter_char);
	// nums[0] += counter_lines;
	// nums[1] += counter_word;
	// nums[2] += counter_char;
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
