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

int invalidFormat(char arr[]);	// checks invalid format
int invalidFirstLine(char arr[]);	// checks invalid first Line

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
	char *check = fgets(buffer,MAXLINE,fileptr);//# Was ist, falls die Zeile mehr als MAXLINE Zeichen beinhaltet?
	if(check == NULL){
		fprintf(stderr, RED BOLD "ERROR: Read-Error\n");
		return(1);
	}
	// invalid format?
	if(invalidFirstLine(buffer)){
		fprintf(stderr, RED BOLD "ERROR: Format-Error\n");
		return(1);
	}

	// convert stringNumber to integer
	number = strtoimax(buffer, NULL, 10);
	// printf("str: \t %s", buffer);
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

		if(invalidFormat(buffer)){
			fprintf(stderr, RED BOLD "ERROR: Format-Error\n");
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

/*	first line is valid <-> contains only digits
	and nothing else
	OR
	<-> empty <->
*/
int invalidFirstLine(char arr[]){
	for(unsigned int i = 0; i < strlen(arr); ++i){
		if((48 <= arr[i] && arr[i] <= 57) ||
			(i != 0 && arr[i] =='\n') ) continue;
		else if(arr[i] == '\0') break;
		else return(1);
	}
	return(0);
}



/*	format is valid
		1) chars are digits and '*' WITHOUT \n or \t
		2) only one '*' exists
		3) first and last entry is digit and not a '*'
*/
int invalidFormat(char arr[]){
	int starCounter = 0;	// counter for '*'
	int flagA = 0; 			// exists a of a*b?
	int flagB = 0; 			// exists b of a*b?


	for(unsigned int i = 0; i < strlen(arr); ++i){
		// ENTER
		if(arr[i] == '\n'){
			// invalid #'*'
			if(starCounter != 1 || !flagA || !flagB)
				return(1);
			// it is still possible that this exists: 121*\n
			else if(i != 0 && arr[i-1] == 42)
				return(1);
			// ok
			else 	return(0);
		}
		// 1) digits
		else if(48 <= arr[i] && arr[i] <= 57){
				if(!flagA)			flagA = 1;
				else if(!flagB) 	flagB = 1;

				continue;
		}
		// 1) '*'
		else if(42 == arr[i]){
			starCounter++;
			// 2) & 3)
			if(starCounter > 1 || i == 0)
				return(1);
		}
		else if(arr[i] == '\0') return(0);
		// other invalid chars
		else return(1);
	}

	// irrelevant
	return(0);
}
