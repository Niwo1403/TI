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

#define PORT 8888

int main(int argc, char *argv[])
{
    //Argumente checken
    if (argc < 2)
        return 1;

    //IP bekommen und in serveradr schreiben
    argv++;
    struct sockaddr_in servaddr;
    if (inet_aton(*argv, &(servaddr.sin_addr))){
        perror("IP konnte nicht gelesen werden.");
        return 1;
    }

    //UDP verbinden
    int sockfd;
    char *buf = malloc(4096);

    //Socket erstellen
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket konnte nicht erstellt werden.");
        return 1;
    }

    //Serverinformationen eingeben //###
    /*servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;*/

    //Senden / Empfangen
    //Paket senden
    int len, last;
    sendto(sockfd, (const char *)argv[0], strlen(argv[0]), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    //Text erhalten und ausgeben
    do
    {
        last = recvfrom(sockfd, (char *)buf, 4096, 0, (struct sockaddr *)&servaddr, (socklen_t *)&len);
        printf("%s", buf);
    } while (last < 4096);

    //Verbindungen etc. schließen
    free(buf);
    close(sockfd);
    return 0;
}
