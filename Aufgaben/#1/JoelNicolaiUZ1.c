// Nicolai & Joel


#include <stdio.h>
#include <inttypes.h>	// strtoimax
#include <string.h>		// memset

// Colours
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

// Maximum Character of one line
#define MAXLINE 250

int main(int argc, char *argv[]){

	// Missing arguments
	if(argc < 2){
		fprintf(stderr, RED BOLD "ERROR: Missing arguments!\n" RESET);
		return(1);
	}

	// open File
	FILE *fileptr = fopen(argv[1], "r");

	// ERROR: Can not open file
	if(fileptr == NULL){
		fprintf(stderr, RED BOLD "ERROR: File-Error\n");
		return(1);
	}

	// Variables
	char buffer[MAXLINE];		// stores fread-input
	int number = 0;				// first line
	int  a, b;					// two factors				

	// read first line
	char *check = fgets(buffer,MAXLINE,fileptr);
	if(check == NULL){
		fprintf(stderr, RED BOLD "ERROR: Read-Error\n");
		return(1);
	}
	// convert stringNumber to integer
	number = strtoimax(buffer, NULL, 10);
	printf("str: \t %s", buffer);
	printf("int: \t %d\n\n", number);

	// // Read till the end of file
	while(! feof(fileptr)){	
		// read line per line
		check = fgets(buffer, MAXLINE, fileptr);

		if(feof(fileptr)) break;
		// printf("newline: %s", buffer);

		if(check == NULL){
			fprintf(stderr, RED BOLD "ERROR: Read-Error\n");
			return(1);
		}

		if(feof(fileptr)) break;
		// printf("newline: %s", buffer);

		char *endptr;	// string which can not be converted 
	    a = strtoimax(buffer, &endptr, 10);

		// correction of string, bc there is just a * 
		// turn * to zero
		memset(endptr, '0', 1);
	    b = strtoimax(endptr, NULL, 10);
		printf("a: \t %d\tb: \t %d\n", a,b);

		// check if a * b == number
		if(a * b == number) continue;
		else{
			printf("return(1)\n");
			return(1);
		}

	}
	// close file
	fclose(fileptr);
	printf("return(0)\n");
	return(0);
}

