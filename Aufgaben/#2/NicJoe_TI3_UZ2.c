/*	Nicolai Wolfrom & Joel Heuer 
	TI III - Uebung #2 Programmierung
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

	if(argc < 2){
		write(1, "ERROR: Missing arguments!\n", myStrlen("ERROR: Missing arguments!\n"));
		return(1);
	} 	
	else if(argc == 2)  a = trashcan(argv[1], "/");
	else 				a = trashcan(argv[1], argv[2]);

	// if(a) write(1,"Fehler\n", 8);

    return(a);
}

int copy(char *sourcename, char *targetname){
	int fdSource;		// filedesc fuer Quelle
	int fdTryTarget;	// filedesc prueft ob Kopie schon existiert
	int fdTarget;		// filedesc fuer neue Kopie
	char buffer[PUFFERSIZE];		// puffer für read
	int checkRead = 0, checkWrite = 0; 	// wie viele Bytes wurden gelesen/geschrieben

	// pruefen, ob Quelle existiert
//	write(1,sourcename,myStrlen(sourcename));
//	write(1,"\n",2);
	fdSource = open(sourcename, O_RDONLY);
	if(fdSource < 0){

		write(1, "ERROR: Can't open sourcefile!\n", myStrlen("ERROR: Can't open sourcefile!\n"));
		return(1); 
	} 
	// pruefen, ob Ziel schon existiert
//	write(1,targetname,myStrlen(targetname));
//	write(1,"\n",2);
	fdTryTarget = open(targetname, O_WRONLY);
	// falls nein, Datei erstellen
	if(fdTryTarget < 0){
		fdTarget = open(targetname, O_CREAT | O_WRONLY, 0600);
		while(1){
			// Quelle lesen 
			checkRead = read(fdSource, buffer, PUFFERSIZE);
			if(checkRead < 0){
				write(1, "ERROR: Can't read sourcefile!\n", myStrlen("ERROR: Can't read sourcefile!\n"));
				return(1);
			} 
			else if(checkRead == 0) break;	// Falls am Ende der Datei, stopppe
			do{							// sonst in Ziel hineinschreiben
				// so viel schreiben, wie gelesen wurde
				checkWrite += write(fdTarget, buffer, checkRead);	
			}while(checkRead != checkWrite);//###ist das do whie nicht unnötig? ersetzt ein while (checkRead != checkWrite) nicht das elseif und else ? 
			/*###
			also:
				while (checkRead != checkWrite){
					checkWrite += write(fdTarget, buffer, checkRead);
				}
				break;
			anstatt:
				else if(checkRead == 0) break;	// Falls am Ende der Datei, stopppe
				do{							// sonst in Ziel hineinschreiben
					// so viel schreiben, wie gelesen wurde
					checkWrite += write(fdTarget, buffer, checkRead);	
				}while(checkRead != checkWrite);
			*/
			
			checkRead = 0;
			checkWrite = 0;
		}

		// Quelle & Ziel schließen
		close(fdSource);
		close(fdTarget);
		return(0);
	}
	// falls ja, beende 
	write(1, "ERROR: targetfile already exists!\n", myStrlen("ERROR: targetfile already exists!\n"));
	return(1);
}
// MAIN-ENDE

int trashcan(char *mode, char *file){	// return 1 == FAIL,	return 0 == SUCCESS
	char *trashcanName = ".trashcan";//###alternativ /.trashcan, dann musst du nicht immer strcat nutzen...
	int check;	// fuer mkdir, memset (erfolgreich?)

	// Papierkorb erstellen, falls nicht vorhanden
	// mit folgenden Berechtigungen: Lesen, Schreiben, Suchen [Besitzer & Gruppe]
	// 								 Lesen, suchen 			  [Andere]
	check = mkdir(trashcanName, 0755 /*| S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH*/); // sind paar Berechtigungen, 
									// aber keine Ahnung wie man konkret sieht welche eine Datei hat.
	// if(check == -1) return(1);	// Ordner existiert schon

	// CHECK FOR VALID FILE-NAME
		// --> "/" is invalid filename

	// aktuelles Verzeichnis bzw. Path
	char path[PATHSIZE];
	char *checkPath = getcwd(path, sizeof(path));
	if(checkPath == NULL){
		write(1, "ERROR: Can't find path!\n", myStrlen("ERROR: Can't find path!\n"));
		return(1);
	} 
    //  if (getcwd(path, sizeof(path)) == NULL) // Fehler
    	//return(1);	

	//write(1,mode,myStrlen(mode));
	//write(1,", ",3);
	//write(1,mode,myStrlen(mode));
	//write(1,"\n",2);

	// DELETE
	if(strcmp(mode, "-d") == 0 && strcmp(file, "/") != 0){
		 // Path-Wechsel: Papierkorb
	    strcat(path, "/");
	    strcat(path, trashcanName);

		// Was soll in den Papierkorb?
		strcat(path, "/");
		strcat(path, file);//### falls der Pfad ./a/b/c/file.txt ist, wird der Trashcan pfad ./trashcan/a/b/c/file.txt, werden die Ordner automatisch erstellt
	
		// kopiere in den Papierkorb
		copy(file, path);

		// loesche original
		check = unlink(file);
		if(check == -1){
			write(1, "ERROR: Can't delete file!\n", myStrlen("ERROR: Can't delete file!\n"));
			return(1);//###ist nur -1 ein Fehler? alternetiv "return check;"...
		} 
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
    	if(!directory){
    		write(1, "ERROR: Can't open trashcan!\n", myStrlen("ERROR: Can't open trashcan!\n"));
	    	return(1);	// Fehler
    	} 
    	
    	// solange auslesen, bis alle Dateien ausgelesen
    	while ((dir = readdir(directory)) != NULL){
			write(1,dir->d_name,myStrlen(dir->d_name));
			write(1,"\n",2);
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
	//	write(1,"TRASHCAN: \t ",13);
	//	write(1,pathTrashcan,myStrlen(pathTrashcan));
	//	write(1,"\n",2);
		// printf("TRASHCAN: \t %s\n", pathTrashcan);

		// pruefen, ob Datei existiert
		check = open(pathTrashcan, O_RDONLY);
		if(check < 0){
			write(1, "ERROR: Can't open file! Maybe file does not exist.\n", 
				myStrlen("ERROR: Can't open file! Maybe file does not exist.\n"));
			return(1);
		} 

		// FUER RECOVOVER
		if(strcmp(mode, "-r") == 0){
			// Datei existiert 
			// Datei im Verzeichnis wiederherstellen
			close(check);
			strcat(path, "/"); strcat(path, file);
		//	write(1,"PATH: \t\t ", 10);
		//	write(1,path,myStrlen(path));
		//	write(1,"\n", 2);
			// printf("PATH \t\t %s\n", path);
			copy(pathTrashcan, path);
		}
		// RECOVER ENDE

		// und im Papierkorb loeschen
		unlink(pathTrashcan);//###Leere Ordner dürfen über bleiben oder sollen die auch gelöscht werden, bzw. werden die automatisch gelöscht?
		return(0);//###falls trashcan nun leer ist, soll der Ordner dann gelöscht werden oder wid es vllt. automatisch?
	}
	write(1, "ERROR: wrong instruction!\nTry\t-d file\n\t-r file\n\t-f file\n\t-l\n",
			 myStrlen("ERROR: wrong instruction!\nTry\t-d file\n\t-r file\n\t-f file\n\t-l\n"));
	return(1);
}

int myStrlen(char p[]){//###Funktioniert es so? Array ist eig. nicht deklariert, es fahlt die Länganangabe...->Pointer
	int len = 0;
	
	for(int i = 0; p[i] != '\0'; ++i){
		len++;
	}
	return(len+1);
}
