// mycat von Nicolai und Joel

#include <stdio.h>
#include <sys/types.h>	// pid_t
#include <sys/wait.h>	// wait(int *)
#include <unistd.h>		// fork()
#include <stdlib.h>		// exit(int)
#define SIZE 256

int my_isspace(char);
int my_cat(char *);

int main(int argc, char *argv[]){
	int check = 0;			// Flag ob my_cat funktioniert

	if(argc < 2){ // Zu wenige Argumente
		/*fprintf(stderr, "%s\n", "cat-ERROR: Missing arguments");
		return(1); */
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
		check = my_cat(newbuffer);	
	}
	else if(argc == 2) check = my_cat(argv[1]);
	else{ /* argc > 2 */
		// Erzeuge Prozess-Array wie argc groß ist
		pid_t pids[argc-1];
		int status[argc-1]; 	// für wait();

		// erzeuge neue KIndprozesse mit for-loop
		for(int i = 0; i < argc-1; ++i){
			// Neuer Kindprozess
			pids[i] = fork();
			if(pids[i] == 0){
				check = my_cat(argv[i+1]);
				if(check){
					fprintf(stderr, "ERROR: child-process %d\n", i);
				} 
				// Kindprozess i beenden
				exit(1);
			}
			// Elternprozess wartet bis Kindprozesse der Reihe nach beendet sind
			wait(&status[i]);
		} // for
	}

	return(check);
}
int my_cat(char *arr){
	int c; 		// char der gelesen wird
	fprintf(stdout, "%s\n", arr);	// Fuer Pipe-Umleitung , die fuer b) gefordert ist.
						// '\n' ist wichtig!
	// ist die Datei gueltig?
	FILE *filepointer = fopen(arr, "r");
	if(filepointer == NULL){
		fprintf(stderr, "my_cat-ERROR: Can't open file!\n");// Kann Datei nicht öffnen
		return(1);
	}
	// solange lesen bis end_of_file erreicht
	while(c = fgetc(filepointer), c != EOF){
		fprintf(stdout, "%c", c);
	}
	// Datei wieder schließen	
	fclose(filepointer);
	return(0);
}

int my_isspace(char x){
	return( x == 32 || x == '\t' || x == '\n' || 
	    	x == '\f'  || x == '\r' || x == '\v');
}
