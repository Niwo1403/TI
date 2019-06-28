#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BIT_MASK 0x8005//gegeben (Das Polynom)

int bin_len(int);
int crc16(int);

int main(int argc, char *argv[]){
    if (argc < 2)
        return 1;

    int len = strlen(argv[1]), vgl_wert = -1;
    if (len > 4 && strcmp(argv[1]+len-4, ".crc") == 0){
        FILE *file_crcr = fopen(argv[1], "r");
        if (file_crcr == NULL)
            return 1;
        //File mit Orginalnamen erstellen
        *(argv[1]+len-4) = '\0';
        FILE *file_crcw = fopen(argv[1], "w");
        if (file_crcr == NULL)
            return 1;

        //crc Wert bekommen und file ohne crc Wert als temporären ersatz zum Berechnen nutzen
        char *buf = malloc(4096);//4096 als Maxlänge pro Zeile
        char *buf_start = buf;
        *buf = '\0';
        while (*buf != EOF){
            buf = buf_start;//Pointer zurücksetzen
            fwrite(buf, 1, strlen(buf), file_crcw);
            *buf = getc(file_crcr);
            while (*buf != '\n' && *buf != EOF){
                buf++;
                *buf = fgetc(file_crcr);
            }
            *(buf+1) = '\0';//String terminieren
        }
        *buf = '\0';//String terminieren
        vgl_wert = *buf_start == '-'? 0-atoi(buf_start+1) : atoi(buf_start);
        
        //Pointer freigeben und schließen
        fclose(file_crcr);
        fclose(file_crcw);
        free(buf_start);//free für buf
    }

    //Filepointer zu Quelldatei erstellen
    FILE *file_src = fopen(argv[1], "r");
    if (file_src == NULL)
        return 1;

    //Src lesen
    int *content = malloc(0);
    char buf[1];
    int elements = 0;
    while ((len = fread(buf, 1, 1, file_src)) > 0){
        elements += len;
        content = realloc(content, elements*sizeof(int));
        *(content+elements-1) = (int)*buf;//Element ans Ende anhängen
    }
    elements++;
    content = realloc(content, elements*sizeof(long));
    *(content+elements-1) = 0;//noch ein Element aus Nullen ran hängen
    //File schließen
    fclose(file_src);


    //crc errechnen
    
    long rest = 0; 
    for (int i = 0; i < elements; i++){
        if ( ((rest >> 15) & 1) != *content)
            rest = (rest << 1) ^ BIT_MASK;
        else
            rest = (rest << 1);
        content++;
    }
    //Poiner zurücksetzen
    content -= elements;
    //rest ist das Ergebnis

    //Test ob crc Wert stimmt oder Dokument mit crc Wert schreiben
    if (vgl_wert >= 0){
        //testen ob crc file richtig ist
        if(vgl_wert != rest){
            //Wert stimmt nicht
            unlink(argv[1]);
            printf("Error, falscher crc Wert.\n");
        }//bei else stimmt Wert, File mit Orginalnamen muss also nicht entfernt werden
    }else{
        //.crc file zuende schreiben (rest bzw. crc Wert hinzufügen)
        char *path = malloc(strlen(argv[1]+5));
        strcpy(path, argv[1]);
        strcat(path, ".crc");
        FILE *file_dest = fopen(path, "wb");
        free(path);
        if (file_dest == NULL)
            return 1;
        //Inhalt in dei neue Datei schreiben
        for (int i = 1; i < elements; i++){
            fwrite(content, 4, 1, file_src);
            content++;
        }
        //rest ans Ende schreiben
        char ias[23];//int als String
        sprintf(ias, "\n%ld", rest);
        fwrite(ias, 1, strlen(ias), file_src);
        //dest file schließen
        fclose(file_dest);
    }

    return 0;
}

int bin_len(int num){
    int count = 0;
    while (num > 0){
        count ++;
        num = num >> 1;
    }
    return count;
}