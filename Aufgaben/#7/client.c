#include <stdio.h>
#include <fcntl.h>//vorgegeben
#include <sys/mman.h>//vorgegeben
//#include <sys/types.h>
#include <sys/ipc.h>//für key_t
#include  <sys/shm.h>

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

    //Text lesen
    fgets(memory, size, stdin);//ließt den Text und schreibt in an die "shared" Adresse 

    //memory beenden bei client beenden
    if (shmdt((void *)memory) == -1){
        printf("Error bei shmdt.\n");
        return 1;
    }
    return 0;
}