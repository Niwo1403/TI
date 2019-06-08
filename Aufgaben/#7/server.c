#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>//vorgegeben
#include <sys/mman.h>//vorgegeben
//#include <sys/types.h>
#include <sys/ipc.h>//für key_t
#include  <sys/shm.h>
#include <string.h>

int main(){
    //key bekommen
    key_t key;
    size_t size = 1024;
    key = ftok("/nicolai/Schreibtisch/TI/Aufgaben/#7/file", 'R');

    //id bekommen
    int sm_id = shmget(key, size, 0666 | IPC_CREAT);//entspricht -rw-rw-rw-

    //Pointer zu memory bekommen
    char *memory = (char *)shmat(sm_id, (void *)0, 0);//char um bei Pointerarithmetik immer um 1 zu verschieben
    if (memory == (char *)-1){//test ob Fahler
        printf("Fehler bei shmat.\n");
        return 1;
    }

    //Text schreiben, falls er geändert wurde
    char *cpy = malloc(size);
    strcpy(cpy, memory);
    while (strcmp(cpy, memory)==0){//auf Änderung warten
        sleep(0.05);//50 Milllisekunden jeweils warten
    }
    printf("%s", memory);//newLine nicht nötig, da sie mitgesendet wird

    //memory beenden bei Server beenden
    if (shmdt((void *)memory) == -1){
        printf("Error bei shmdt.\n");
        return 1;
    }

    //memory complett beenden
    shmctl(sm_id, IPC_RMID, NULL);

    return 0;
}