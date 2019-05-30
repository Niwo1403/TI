#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

struct ll{
    char *t;
    struct ll *next;
};
int mode = 0; //mode 0: , mode 1: a, mode 2: l, mode 3: l, a

int ls(char *);
int print_file_infos(char *);
void sortll(struct ll *);

int main(int argc, char *argv[]){
    //Argumente auslesen -> Dateien/Ordner und modifier (l, a)
    int ret = 0;
    for (int i = 1; i < argc && ret == 0; i++){
        if (*argv[i] == '-'){
            char *zs = argv[i];
            argv[i]++;
            while (*argv[i] != '\0' && ret == 0){//Modifier lesen
                if (*argv[i] == 'a' && mode%2==0){//test ob es l ist und ob l noch nicht gesetzt wurde
                    mode++;
                }else if (*argv[i] == 'l' && mode<2){
                    mode += 2;
                }else{//fehlerhafte Perameter
                    ret = 2;
                }
                argv[i]++;
            }
            argv[i] = zs;//Pointer zurücksetzen
            *argv[i] = '\0'; //ab jetzt kann für Elemente von argv strlen(argv[i]) als boolean gewertet werden, der sagt ob es sich um eine Datei oder einen Ordner handelt
        }
    }
    //über Dateien/Ordner iterieren:
    char *cwd = getcwd(NULL, 0);
    int done_nthng = 1;//guckt ob eine Datei oder ein Ordner angegeben wurde
    for (int i = 1; i < argc && ret == 0; i++){
        if (strlen(argv[i])){ //funktioniert, da -l etc durch \0 ersetzt wurde
            if (! done_nthng){
                printf("\n");
            }
            printf("%s:\n", argv[i]);
            ret = ls(argv[i]);
            if (done_nthng){
                done_nthng = 0;
            }
        }
    }
    if(done_nthng && ret == 0){ //falls keine Datei oder Ordner übergeben wurde
        printf("%s:\n", cwd);
        ret = ls(".");
    }
    printf("\n");

    return ret;
}

int ls(char *path){
    struct stat statbuffer;//für Filetest
    int ret = 0;
    stat(path, &statbuffer);
    if (! S_ISDIR(statbuffer.st_mode)){//test ob File oder Ordner
        //File
        ret = print_file_infos(path);
    }else{
        //Ordner
        DIR *d;
        struct dirent *dir;
        d = opendir(path);
        if (d){
            //dirs lesen
            struct ll *first = malloc(sizeof(struct ll));//um den Anfang zu speichern
            if (first == NULL)
                return 2;
            struct ll *list = first;
            list->t = "";//Pseudoelement, relevat für späteres sortieren
            list->next = NULL;
            while ((dir = readdir(d)) != NULL){
                list->next = malloc(sizeof(struct ll));
                if (list->next == NULL)
                    return 2;
                list->next->t = dir->d_name;
                list->next->next = NULL;
                list = list->next;
            }
            //Linkedlist sortieren
            sortll(first);
            list = first->next;
            //Infos ausgeben
            while (list != NULL && ret == 0){
                if (*(list->t) != '.' || mode%2 == 1){ //falls Datei versteckt, muss a als Modifier übergeben worden sein
                    char *a = malloc(strlen(path)+strlen(list->t)+2);
                    strcpy(a, path);
                    int last_index = strlen(path)-1;
                    path += last_index;
                    if (*path == '/'){//test ob path von Ordner / hinten hat
                        path -= last_index;
                    }else{
                        strcat(a, "/");
                        path -= last_index;
                    }
                    strcat(a, list->t);
                    ret = print_file_infos(a);
                }
                list = list->next;
            }
            closedir(d);
            free(dir);
        }else{//Ordner konnte nicht geöffnet werden
            ret = 1;
        }
    }
    return ret;       
}

int print_file_infos(char *path){
    if (mode > 1){//l wurde übergeben
        struct stat eigenschaften;//kein Pointer, damit es nicht initialisiert werden muss (Speicher ist so schon reserviert)
        if (stat(path, &eigenschaften) >= 0){
            //Lese, Schreib und Ausführrechte ausgeben: (falls Ausdruck true st wird Bst ausgegeben, sonst -)
            printf((S_ISDIR(eigenschaften.st_mode)) ? "d" : "-");
            printf((eigenschaften.st_mode & S_IRUSR) ? "r" : "-");
            printf((eigenschaften.st_mode & S_IWUSR) ? "w" : "-");
            printf((eigenschaften.st_mode & S_IXUSR) ? "x" : "-");
            printf((eigenschaften.st_mode & S_IRGRP) ? "r" : "-");
            printf((eigenschaften.st_mode & S_IWGRP) ? "w" : "-");
            printf((eigenschaften.st_mode & S_IXGRP) ? "x" : "-");
            printf((eigenschaften.st_mode & S_IROTH) ? "r" : "-");
            printf((eigenschaften.st_mode & S_IWOTH) ? "w" : "-");
            printf((eigenschaften.st_mode & S_IXOTH) ? "x" : "-");
        
            struct tm *tm;
            char time_str[20];
            tm = localtime(&eigenschaften.st_mtime);
            strftime(time_str, sizeof(time_str), "%d.%m.%Y %H:%M:%S", tm);

            printf("\t%ld\t%s\t%s\n", eigenschaften.st_size, time_str, path);
            return 0;
        }else{
            return 1;
        }
    }else{//l wurde nicht übergeben
        printf("%s   ", path);//nur Datei ausgeben
        return 0;
    }
}

void sortll(struct ll *list){//Da Linkedlist -> Bubblesort
    struct ll *tmp, *start = list;
    int count = 0;
    //Größtes Element ans Ende bringen und Länge der Liste bestimmen
    while(list->next->next != NULL){
        if (strcmp(list->next->t, list->next->next->t) > 0){//Test ob getauscht werden mussf
            tmp = list->next;
            list->next = list->next->next;
            tmp->next = list->next->next;
            list->next->next = tmp;
        }
        list = list->next;
        count++;
    }
    //restliche Liste sortieren
    for (int i = 1; i < count; i++){//i=1, da das Größte Element  bereits am Ende ist, n-2 mal noch ausführen
        list = start;//list wieder auf erstes Element setzen
        for (int j = i; j < count; j++){//while Schleife ist nihct nötig, da die Länge die sotiert werden soll bekannt ist
            if (strcmp(list->next->t, list->next->next->t) > 0){//Test ob getauscht werden mussf
                tmp = list->next;
                list->next = list->next->next;
                tmp->next = list->next->next;
                list->next->next = tmp;
            }
            list = list->next;
        }
    }
}