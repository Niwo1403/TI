#include <sys/stat.h>	// mkdir & open
#include <sys/types.h>	// mkdir
#include <sys/stat.h>	// open
#include <fcntl.h>		// open
#include <unistd.h> // write, unlink, read

#define PUFFERSIZE 10	// groeße des Puffer-Arrays fuer Read

/*
int mkdir(const char *pathname, mode_t mode);

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);

ssize_t read (int fd, void * buf, size_t count);

int unlink (const char * Pfadname );

ssize_t write(int fd, const void *buf, size_t count);

*/


// FÜR DEBUGGING
#include <stdio.h>
// spaeter loeschen

int copy(char *sourcename, char *targetname);
int trashcan();


int main(void){

	// char buf[10];

	// int filedesc = open("testfile.txt",
	// 				    O_CREAT | O_RDONLY, 0600);
	// 				   // S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
 //    if(filedesc < 0){
 //    	fprintf(stderr, "fail\n");
 //        return 1;
 //    }
 //    fprintf(stdout, "Try to read\n");
 //    int a = read(filedesc, buf, 3);
 //    printf("%s\n", buf);
 //    close(filedesc);

	char *source = "source.txt";
	char *target = "copy.txt";

	int check = copy(source, target);
	if(check){
		printf("ERROR: COPY\n");
		return(1);
	}

    return(0);
}

int copy(char *sourcename, char *targetname){
	int fdSource;		// filedesc fuer Quelle
	int fdTryTarget;	// filedesc prueft ob Kopie schon existiert
	int fdTarget;		// filedesc fuer neue Kopie
	char buffer[PUFFERSIZE];		// puffer für read
	int checkRead = 0, checkWrite = 0; 	// wie viele Bytes wurden gelesen/geschrieben

	// pruefen, ob Quelle existiert
	printf("%s\n", sourcename);
	fdSource = open(sourcename, O_RDONLY);
	if(fdSource < 0) return(1); 

	// pruefen, ob Ziel schon existiert
	printf("%s\n", targetname);
	fdTryTarget = open(targetname, O_WRONLY);
	// falls nein, Datei erstellen
	if(fdTryTarget < 0){
		fdTarget = open(targetname, O_CREAT | O_WRONLY, 0600);
		while(1){
			// Quelle lesen 
			checkRead = read(fdSource, buffer, PUFFERSIZE);
			if(checkRead < 0) return(1);	// ERROR
			// fprintf(stdout, "seen: %d\n", checkRead);
			else if(checkRead == 0) break;	// Falls am Ende der Datei, stopppe
			do{							// sonst in Ziel hineinschreiben
				// so viel schreiben, wie gelesen wurde
				checkWrite += write(fdTarget, buffer, checkRead);	
				// fprintf(stdout, "written: %d\n", checkWrite);
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

int trashcan(){		// return 1 == FAIL,	return 0 == SUCCESS


	return(1);
}