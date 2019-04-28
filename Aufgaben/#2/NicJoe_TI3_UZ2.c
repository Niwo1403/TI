/*	Nicolai Wolfrom & Joel Heuer 
	TI III - Uebung #2 Programmierung



	TO DO:
		1) 	Gescheite Fehlermeldungen + Begruendung 
			ohne <stdio.h> & <stdlib.h>

		2) 	Ordner trashcan unsichtbar machen koennen

		3) 	LIST-Funktion kann bis jetzt noch nix auflisten
			weil wir prinf() nicht nutzen ḱoennen
*/

#include <sys/stat.h>	// mkdir & open
#include <sys/types.h>	// mkdir
#include <sys/stat.h>	// open
#include <fcntl.h>		// open
#include <unistd.h> 	// write, unlink, read
#include <libgen.h>		// dirname
#include <string.h>		// strcat, strncpy, strcmp
#include <dirent.h> 	// readdir

#define PUFFERSIZE 10	// groeße des Puffer-Arrays fuer Read in copy()
#define PATHSIZE 100	// große fuer Array, das Pfad speichert

// FÜR DEBUGGING
// #include <stdio.h>

// FUNKTIONEN
int copy(char *sourcename, char *targetname);
int trashcan(char *mode, char *file);
int myStrlen(char p[]);

// MAIN
int main(int argc, char *argv[]){

	int  a;

	if(argc < 2) 		return(1);
	else if(argc == 2)  a = trashcan(argv[1], "/");
	else 				a = trashcan(argv[1], argv[2]);

	// if(a) printf("Fehler\n");

    return(a);
}

int copy(char *sourcename, char *targetname){
	int fdSource;		// filedesc fuer Quelle
	int fdTryTarget;	// filedesc prueft ob Kopie schon existiert
	int fdTarget;		// filedesc fuer neue Kopie
	char buffer[PUFFERSIZE];		// puffer für read
	int checkRead = 0, checkWrite = 0; 	// wie viele Bytes wurden gelesen/geschrieben

	// pruefen, ob Quelle existiert
	// printf("%s\n", sourcename);
	fdSource = open(sourcename, O_RDONLY);
	if(fdSource < 0) return(1); 

	// pruefen, ob Ziel schon existiert
	// printf("%s\n", targetname);
	fdTryTarget = open(targetname, O_WRONLY);
	// falls nein, Datei erstellen
	if(fdTryTarget < 0){
		fdTarget = open(targetname, O_CREAT | O_WRONLY, 0600);
		while(1){
			// Quelle lesen 
			checkRead = read(fdSource, buffer, PUFFERSIZE);
			if(checkRead < 0) return(1);	// ERROR
			else if(checkRead == 0) break;	// Falls am Ende der Datei, stopppe
			do{							// sonst in Ziel hineinschreiben
				// so viel schreiben, wie gelesen wurde
				checkWrite += write(fdTarget, buffer, checkRead);	
			}while(checkRead != checkWrite);
			checkRead = 0;
			checkWrite = 0;
		}

		// Quelle & Ziel schließen
		close(fdSource);
		close(fdTarget);
		return(0);
	}
	// falls ja, beende 
	return(1);
}
// MAIN-ENDE

int trashcan(char *mode, char *file){	// return 1 == FAIL,	return 0 == SUCCESS
	char *trashcanName = ".trashcan";
	int check;	// fuer mkdir, memset (erfolgreich?)

	// Papierkorb erstellen, falls nicht vorhanden
	// mit folgenden Berechtigungen: Lesen, Schreiben, Suchen [Besitzer & Gruppe]
	// 								 Lesen, suchen 			  [Andere]
	check = mkdir(trashcanName, 0755 /*| S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH*/);
	// if(check == -1) return(1);	// Ordner existiert schon

	// CHECK FOR VALID FILE-NAME
		// --> "/" is invalid filename

	// aktuelles Verzeichnis bzw. Path
	char path[PATHSIZE];
    if (getcwd(path, sizeof(path)) == NULL) // Fehler
    	return(1);	

    // printf("%s, %s\n", mode, file);
	// DELETE
	if(strcmp(mode, "-d") == 0 && strcmp(file, "/") != 0){
		 // Path-Wechsel: Papierkorb
	    strcat(path, "/");
	    strcat(path, trashcanName);

		// Was soll in den Papierkorb?
		strcat(path, "/");
		strcat(path, file);
	
		// kopiere in den Papierkorb
		copy(file, path);

		// loesche original
		check = unlink(file);
		if(check == -1) return(1);
		return(0);
	}
	// LIST
	else if(strcmp(mode, "-l") == 0 && strcmp(file, "/") == 0){
    	
    	struct dirent *dir;
		DIR *directory;

   		 // Path-Wechsel: Papierkorb
	    strcat(path, "/");
	    strcat(path, trashcanName);
	    
	    // Ordner oeffnen
    	directory = opendir(path);
    	if(directory != 0) return(1);	// Fehler
    	
    	// solange auslesen, bis alle Dateien ausgelesen
    	while ((dir = readdir(directory)) != NULL){
            // printf("%s\n", dir->d_name);
        }
        closedir(directory);
    	return(0);
	}

	// RECOVER && FINALLY DELETE
	else if((strcmp(mode, "-r") == 0 || strcmp(mode, "-f") == 0)
			&& strcmp(file, "/") != 0){
		// kopiere aktuellen Pfad
		char pathTrashcan[PATHSIZE];
		strncpy(pathTrashcan, path, myStrlen(path));

		// Path-Kopie: gehe in den Papierkorb
		strcat(pathTrashcan, "/");
		strcat(pathTrashcan, trashcanName);
		strcat(pathTrashcan, "/");
		strcat(pathTrashcan, file);
		// printf("TRASHCAN: \t %s\n", pathTrashcan);

		// pruefen, ob Datei existiert
		check = open(pathTrashcan, O_RDONLY);
		if(check < 0) return(1);

		// FUER RECOVOVER
		if(strcmp(mode, "-r") == 0){
			// Datei existiert 
			// Datei im Verzeichnis wiederherstellen
			close(check);
			strcat(path, "/"); strcat(path, file);
			// printf("PATH \t\t %s\n", path);
			copy(pathTrashcan, path);
		}
		// RECOVER ENDE

		// und im Papierkorb loeschen
		unlink(pathTrashcan);
		return(0);
	}
	
	return(1);
}

int myStrlen(char p[]){
	int len = 0;
	
	for(int i = 0; p[i] != '\0'; ++i){
		len++;
	}
	return(len+1);
}
