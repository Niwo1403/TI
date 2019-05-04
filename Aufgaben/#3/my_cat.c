// mycat von Nicolai und Joel

#include <stdio.h>

int main(int argc, char *argv[]){
	if(argc < 2){ // Zu wenige Argumente
		fprintf(stderr, "%s\n", "cat-ERROR: Missing arguments");
		return(1);
	}

	int c; 		// char der gelesen wird
	fprintf(stdout, "%s\n", argv[1]);	// Fuer Pipe-Umleitung , die fuer b) gefordert ist.
						// '\n' ist wichtig!
	// ist die Datei gueltig?
	FILE *filepointer = fopen(argv[1], "r");
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

