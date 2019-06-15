#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
//#include <netdb.h> 
//#include <netinet/in.h> 

int unix_s();
int udp_s();
int tcp_s();

#define PORT 8888

int main(int argc, char *argv[]){
    //test ob Argumente üergeben
    if (argc < 2)
        return 1;
    if (strcmp(argv[1], "-U") == 0){//Unix Socket
        return unix_s();
    }else if (strcmp(argv[1], "-u") == 0){//UDP Socket
        return udp_s();
    }else if (strcmp(argv[1], "-t") == 0){//TCP Socket
        return tcp_s();
    }
    return 0;
}

int unix_s(){
    //Unix Socket erstellen
    int s, s2, t, len;
    struct sockaddr_un local, remote;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {//test ob Socketerstellung erfolgreich
        perror("Socket konnte nicht erstellt werden.");
        return 1;
    }

    //Socket binden
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, "./socket_file");
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("Socket konnte nicht an Datei geunden werden.");
        return 1;
    }

    //Socket starten (Listener starten)
    if (listen(s, 5) == -1) {
        perror("Socket konnte nicht gestartet werden.");
        return 1;
    }

    //Auf Verbindung warten
    t = sizeof(remote);
    if ((s2 = accept(s, (struct sockaddr *) &remote, (socklen_t *)&t)) == -1) {
        perror("Socket konnte Client nicht akzeptieren.");
        return 1;
    }
    //jetzt mit Client verbunden
    
    //Path bekommen und content zurück senden
    char *buf = malloc(4096);//4096 ist eig. max Pfadlänge
    len = recv(s2, buf, 4096, 0);

    //content senden
    FILE *file = fopen(buf, "r");
    if (file == NULL){
        perror("Datei konnte nicht gelesen werden.");
        return 1;
    }
    while ((len = fread(buf, 1, 4096, file)) > 0)
        send(s2, buf, len, 0);

    //Variablen löschen, Verbindungen beenden
    fclose(file);
    close(s2);
    free(buf);
    return 0;
}

int udp_s(){
    int sockfd; 
    char *buf = malloc(4096); 
    struct sockaddr_in servaddr, cliaddr; 
      
    //Socket erstellen
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("Socket konnte nicht erstellt werden."); 
        return 1; 
    }
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    
    //Serverinformationen eingeben
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT); 

    //Socket binden
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) { 
        perror("Socket konnte nicht gebunden werden."); 
        return 1; 
    } 
    
    //Senden / Empfangen
    //Path empfangen
    int len; 
    recvfrom(sockfd, (char *)buf, 4096,  MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
    //content senden
    FILE *file = fopen(buf, "r");
    if (file == NULL){
        perror("Datei konnte nicht gelesen werden.");
        return 1;
    }
    while ((len = fread(buf, 1, 4096, file)) > 0)
        printf("%ld\n", sendto(sockfd, (const char *)buf, strlen(buf)+1, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, sizeof(cliaddr))); 

    //Verbindungen etc. schließen
    free(buf);
    close(sockfd); 
    return 0;
}

int tcp_s(){
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        perror("Socket konnte nicht erstellt werden."); 
        return 1; 
    }
    bzero(&servaddr, sizeof(servaddr)); //ansich ähnlich wie memset
  
    //Serverinformationen eintragen
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    //Socket binden
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        perror("Socket konnte nicht gebunden werden."); 
        return 1; 
    }
  
    //Serverlistening starten
    if ((listen(sockfd, 5)) != 0) { 
        perror("Server listener konnte nicht gestartet werden.");
        return 1;
    }
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    if ((connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t *) &len)) < 0) { 
        perror("Socketverbindung konnte nicht akzeptiert werden."); 
        return 1; 
    }
    //Cient wurde akzeotiert
  
    //Path lesen
    char *buf = malloc(4096); 
    read(connfd, buf, 4096); 
    //content lesen und zurückschicken
    FILE *file = fopen(buf, "r");
    if (file == NULL){
        perror("Datei konnte nicht gelesen werden.");
        return 1;
    }
    while (fread(buf, 1, 4096, file) > 0)
        write(connfd, buf, strlen(buf)+1); 

    // Verbindungen schließen etc.
    free(buf);
    close(sockfd); 
    return 0;
}
