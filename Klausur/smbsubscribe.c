/*
This is the implimentation of Subscriber
Subscriber:
    *consumer / receiver of message
    *connects to Broker and subscribes to Topic
    *reads message from broker for further processing
    
@gogamid Imron Gamidli 
https://github.com/gogamid/TCPIP/tree/main/Klausur
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
    if (argc < 3)
    {
        fprintf(stderr, "Aufruf: smbsubscribe broker topic\n");
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
    char *server_ip = inet_ntoa(*(struct in_addr *)hostptr->h_addr); // Server IP
    // Socket anlegen und oeffnen
    // Familie: Internet, Typ: UDP-Socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // Serverstruktur einrichten
    // Datenstruktur auf 0 setzen
    // Familie: Internetserver
    server_size = sizeof(server_addr);
    memset((void *)&server_addr, 0, server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(srv_port);

    // Nachricht an Server schicken
    sprintf(buffer, "s%s", argv[2]);
    length = strlen(buffer);
    fprintf(stderr, "\nmessage to broker: %s \n", buffer);
    nbytes = sendto(sock_fd, buffer, length, 0, (struct sockaddr *)&server_addr, server_size);
    if (nbytes != length)
    {
        perror("sendto");
        return 1;
    }
    // In Endlosschleife auf Nachrichten vom Broker warten
    while (1)
    {
        // Antwort vom Server lesen
        nbytes = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        if (nbytes < 0)
        {
            perror("recvfrom");
            return 1;
        }
        buffer[nbytes] = '\0';
        fprintf(stderr, "\nmessage from broker: %s \n", buffer);
    }
    return 0;
}