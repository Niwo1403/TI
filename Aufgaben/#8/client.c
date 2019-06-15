#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <strings.h>
//#include <netdb.h> 

int unix_s(char *);
int udp_s(char *);
int tcp_s(char *);

#define PORT 8888

int main(int argc, char *argv[]){
    //test ob Argumente üergeben
    if (argc < 3)
        return 1;
    if (strcmp(argv[1], "-U") == 0){//Unix Socket
        return unix_s(argv[2]);
    }else if (strcmp(argv[1], "-u") == 0){//UDP Socket
        return udp_s(argv[2]);
    }else if (strcmp(argv[1], "-t") == 0){//TCP Socket
        return tcp_s(argv[2]);
    }
    return 0;
}

int unix_s(char *path){
    //Unix Socket erstellen
    int s, len;
    struct sockaddr_un remote;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {//test ob Socketerstellung erfolgreich
        perror("Socket konnte nicht erstellt werden.");
        return 1;
    }

    //Socket binden
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, "./socket_file");
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *) &remote, len) == -1) {
        perror("Socket konnte nicht an Datei gebunden werden.");
        return 1;
    }
    //jetzt mit Server verbunden

    //Path senden
    send(s, path, strlen(path)+1, 0);
    //Inhalte empfangen und ausgeben
    char *buf = malloc(4096);//4096 ist eig. max Pfadlänge
    while ((len = recv(s, buf, 4096, 0)) > 0)
        printf("%s", buf);

    //Variablen löschen, Verbindungen beenden
    close(s);
    free(buf);
    return 0;
}

int udp_s(char *path){
    int sockfd; 
    char *buf = malloc(4096); 
    struct sockaddr_in servaddr; 
  
    //Socket erstellen
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("Socket konnte nicht erstellt werden."); 
        return 1; 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    //Serverinformationen eingeben
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;
      
    //Senden / Empfangen
    //Path senden
    int len, last;
    sendto(sockfd, (const char *)path, strlen(path)+1, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr)); 
    //Text erhalten und ausgeben
    do{
        last = recvfrom(sockfd, (char *)buf, 4096, 0, (struct sockaddr *) &servaddr, (socklen_t *) &len);
        printf("%s", buf); 
    }while (last < 4096);
  
    //Verbindungen etc. schließen
    free(buf);
    close(sockfd); 
    return 0;
}

int tcp_s(char *path){
    int sockfd; 
    struct sockaddr_in servaddr; 

    //Socket erstellen
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        perror("Socket konnte nicht erstellt werden."); 
        return 1; 
    }
    bzero(&servaddr, sizeof(servaddr)); //ansich ähnlich wie memset
  
    //Serverinformationen eintragen
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    //mit Server verbinden
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
        perror("Konnte nicht mit Server verbinden."); 
        return 1; 
    }
  
    //Path senden
    write(sockfd, path, strlen(path)+1); 
    //Inhalt ausgeben
    char *buf = malloc(4096); 
    while (read(sockfd, buf, 4096) > 0)
        printf("%s", buf);
  
    //Verbindung schließen
    free(buf);
    close(sockfd); 

    return 0;
}
