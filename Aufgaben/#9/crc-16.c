#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if (argc < 2)
        return 1;

    //Filepointer zu Quelldatei erstellen
    FILE *file_src = fopen(argv[1], "r+");//r+ um ggf. Checksumme anhängen zu können
    if (file_src == NULL)
        return 1;

    //Src lesen
    int *content = malloc(0);
    char buf[1];
    int elements = 0, len;
    while ((len = fread(buf, 1, 1, file_src)) > 0){
        //für alle Bits eine Element erstelle
        elements += len * 8;
        content = realloc(content, elements*sizeof(int));
        //Char immer um 1 verschoben anhängen
        for(int j = 7; j>=0; j--){
            *(content+elements-1-j) = ((*buf) >> j) & 1;	
        }
    }
    elements += 16;//um Polynomlänge erweitern
    content = realloc(content, elements*sizeof(int));
    //neue Stellen 0 setzen
    for (int i = 0; i < 16; i++)
        content[elements-1-i] = 0;

    //crc errechnen
    //generator Polynom:
	const int polynom[] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
    //für alle gelesen chars Polynomdivision
    for(int i = 0; i<elements-16; i++){
		if(content[i] == 0) //nichts zu tun
            continue;
        //für alle Stellen des chars (und den danach, da das Polynom mehr als 8 Stellen hat)
		for(int j = i; j<i+17; j++)
			content[j] = content[j] ^ polynom[j-i];
	}

    //Ergebnis ausgeben
    if (strlen(argv[1]) && strcmp(argv[1]+strlen(argv[1])-4, ".crc") == 0){
        //.crc Datei
        //test ob richtig (Checksumme == 0):
        int istRichtig = 1;
	    for(int i = elements; i>elements-16; i--)
		    if(content[i] != 0) 
                istRichtig = 0;

        if(istRichtig) 
            printf("Die Datei ist korrekt.\n");
        else
            printf("Die Datei ist nicht korrekt.\n");
	}else{
        //keine .crc Datei
        for(int i = 0; i<2; i++){
			char c = 0;
			for(int j = 0; j<8; j++){
				c = (content[elements-16+i*8+j] << (8-j-1)) ^ c;	
			}
			fwrite(&c, sizeof(char), 1, file_src);
		}

        //Datei umbenennen
        char *new_path = malloc(strlen(argv[1]) + 5);
        strcpy(new_path, argv[1]);
        strcat(new_path, ".crc");
        rename(argv[1], new_path);
        free(new_path);

        printf("Checksumme angehangen.\n");
    }
    //File schließen
    fclose(file_src);

    return 0;
}