/*
UDP write
Nachricht strings: topic (Thema, Betreff) und message (Wert, Inhalt)

Der Publisher wird per Kommandozeile mit topic und message aufgerufen,
schickt die Nachricht an den Broker und terminiert dann. 

Der Aufruf: smbpublish broker topic message

* wobei broker der Hostname oder die IP-Adresse des Brokers ist, 
* topic das Thema / den Betreff der Nachricht und message den Wert / den Inhalt
der Nachricht angibt.
* Wildcards sind dem Publisher nicht erlaubt.
Beispiele:
 smbpublish 127.0.0.1 datum 08.02.2021
 smbpublish smbserver test "hallo welt!"

Topics: 
date
thrmostat
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

// Port
const int srv_port = 8080;

// main
int main(int argc, char **argv)
{
    int sock_fd;                    // Socket
    struct sockaddr_in server_addr; // Serveradresse
    socklen_t server_size;          // Adresslaenge
    char buffer[512];               // Schreib-/Lesepuffer
    int nbytes, length;

    // Kommandozeile bearbeiten
    if (argc < 4)
    {
        fprintf(stderr, "Aufruf: smbpublish broker topic message\n");
        return 1;
    }
    struct hostent *hostptr;
    // hostent-Daten lesen
    if ((hostptr = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "gethostbyname: Host %s nicht gefunden\n\n", argv[1]);
        return 1;
    }

    // 1. Parameter -> IP-Adresse des Server
    char *server_ip = inet_ntoa(*(struct in_addr *)hostptr->h_addr);// Server IP
    // Socket anlegen und oeffnen
    // Familie: Internet, Typ: UDP-Socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    fprintf(stderr, "socket: %d\n", sock_fd);
    if (sock_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // Serverstruktur einrichten
    // Datenstruktur auf 0 setzen
    // Familie: Internetserver
    // Adresse: maya.rz.hs-fulda.de
    server_size = sizeof(server_addr);
    memset((void *)&server_addr, 0, server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(srv_port);

    // Nachricht an Server schicken
    sprintf(buffer,"p %s %s",argv[2], argv[3]);
    length = strlen(buffer);
    fprintf(stderr, "\nmessage---%s---is sent to broker\n", buffer);
    nbytes = sendto(sock_fd, buffer, length, 0, (struct sockaddr *)&server_addr, server_size);
    if (nbytes != length)
    {
        perror("sendto");
        return 1;
    }

    close(sock_fd);
    return 0;
}