#define _POSX_C_SOURCE 20112L
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define POLY_SIZE 16

int crc(int* data, int* poly, int dataSize, int polySize);

int main(int argc, char** argv){

	FILE *inputFile = fopen(argv[1], "r+");
	if(!inputFile){
		fprintf(stderr, "%sInputFile existiert nicht%s\n", "\033[1;31m", "\x1B[0m");
		return 1;
	}
	
	int bitStreamSize = 0;

	char buffer[BUFSIZ];	
	int readBytes;
	while((readBytes = fread(buffer, sizeof buffer[0], sizeof buffer, inputFile))){
		bitStreamSize += readBytes*8;
	}
	
	size_t lenstr = strlen(argv[1]);
	size_t lenCrc = 4;
	int endCrc = strcmp(argv[1]+lenstr-lenCrc, ".crc") == 0;

	int* bitStream;
	if(!endCrc) bitStream = calloc((bitStreamSize+POLY_SIZE), sizeof(int));
	else bitStream = calloc((bitStreamSize), sizeof(int));

	int dataSize = bitStreamSize;
	if(!endCrc) dataSize += POLY_SIZE;

	int bitStreamPos = 0;

	rewind(inputFile);
	while((readBytes = fread(buffer, sizeof buffer[0], sizeof buffer, inputFile))){
		for(int i = 0; i<readBytes; i++){	
			/*printf("%c - ", buffer[i]);
			printf("\n");		
			*/for(int j = 7; j>=0; j--){
				bitStream[bitStreamPos] = (buffer[i] >> j) & 1;			
				bitStreamPos++;
			}
		}
	}

	
	for(int i = 0; i<dataSize; i++){
		printf("%d", bitStream[i]);
		if(i % 8 == 7) printf(" ");
		
	}
	printf("\n");

	
	



	int polyStream[] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
	if(!endCrc) crc(bitStream, polyStream, dataSize-POLY_SIZE, POLY_SIZE+1);
	else crc(bitStream, polyStream, dataSize-POLY_SIZE, POLY_SIZE+1);


	for(int i = 0; i<dataSize; i++){
		printf("%d", bitStream[i]);
		if(i % 8 == 7) printf(" ");
		
	}
	printf("\n");


	int isCorrect = 1;
	for(int i = dataSize; i>dataSize-POLY_SIZE; i--){
		if(bitStream[i] != 0) isCorrect = 0;
	}

	
	

	if(!endCrc){
		for(int i = 0; i<2; i++){
			char c = 0;
			for(int j = 0; j<8; j++){
				c = (bitStream[dataSize-POLY_SIZE+i*8+j] << (8-j-1)) ^ c;	
				printf("%d", bitStream[dataSize-POLY_SIZE+i*8+j]);
			}
			printf("\n");
	
			int resWrite = fwrite(&c, sizeof(char), 1, inputFile);
			printf("%d\n", resWrite);
		}
		
		char* oldPath = malloc((2+strlen(argv[1]))*sizeof(char));
		char* newPath = malloc((6+strlen(argv[1]))*sizeof(char));

		strcat(oldPath, "./");
		strcat(oldPath, argv[1]);

		strcat(newPath, "./");
		strcat(newPath, argv[1]);
		strcat(newPath, ".crc");


		rename(oldPath, newPath);
		free(oldPath);
		free(newPath);
	}else if(isCorrect){
		
		ftruncate(fileno(inputFile), (dataSize/8)-2);
		printf("ERRNO: %d\n", errno);

		size_t nameLen = strlen(argv[1]);
		char* newPath = malloc(nameLen);
		strcpy(newPath, argv[1]);
		newPath[nameLen-4] = '\0';
		rename(argv[1], newPath);

		printf("Datei ist Korrekt\n");
		free(newPath);
	}else{
		printf("Datei ist nicht Korrekt\n");
	}

	free(bitStream);
	fclose(inputFile);
	
	return 0;
} 




int crc(int* data, int* poly, int dataSize, int polySize){
	for(int i = 0; i<dataSize; i++){
		if(data[i] == 0) continue;
		for(int j = i; j<i+polySize; j++){
			data[j] = data[j] ^ poly[j-i];
			
		}
	}
	

	return 0;

}
