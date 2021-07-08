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

/*
UDP read write
Der Subscriber wird per Kommandozeile mit dem ihn interessierenden topic
gestartet und soll ebenfalls in einer Dauerschleife auf Nachrichten vom
Broker warten und diese dann ausgeben

Der Aufruf des Subscribers: smbsubscribe broker topic
wobei broker der Hostname oder die IP-Adresse des Brokers ist
und topic das Thema / den Betreff benennt

examples:
smbsubscribe localhost datum
smbsubscribe 192.168.1.13 "#"

if topic is # then werden alle Nachrichten vom Broker an den
Subscriber weitergeleitet


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

// Port
const int srv_port = 8080;

// Formatstring der Nachricht
#define SERVER_MESSAGE  "Hi, hier Client %d"

// main
int main(int argc, char **argv)
{
  char *server_ip;                      // Server IP
  int sock_fd;                          // Socket
  struct sockaddr_in server_addr;       // Serveradresse
  socklen_t server_size;                // Adresslaenge
  char buffer[512];                     // Schreib-/Lesepuffer
  int nbytes, length;

  // Kommandozeile bearbeiten
  if(argc < 2) {
    fprintf(stderr, "Aufruf: %s Server-IP-Adresse\n", argv[0]);
    return 1;
  }
  // 1. Parameter -> IP-Adresse des Server
  server_ip = argv[1];
  
  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: UDP-Socket
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  fprintf(stderr, "socket: %d\n", sock_fd);
  if(sock_fd < 0) {
    perror("socket");
    return 1;
  }
  
  // Serverstruktur einrichten 
  // Datenstruktur auf 0 setzen
  // Familie: Internetserver
  // Adresse: maya.rz.hs-fulda.de
  server_size = sizeof(server_addr);
  memset((void *) &server_addr, 0, server_size);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(srv_port);
  
  // Nachricht an Server schicken
  sprintf(buffer, SERVER_MESSAGE, getpid());
  length = strlen(buffer);
  fprintf(stderr, "sendto: %d Bytes %s\n", length, buffer);
  nbytes = sendto(sock_fd, buffer, length, 0, (struct sockaddr *) &server_addr, server_size);  
  if(nbytes != length) {
    perror("sendto");
    return 1;  
  }
  
//   // Antwort vom Server lesen
//   nbytes = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
//   if(nbytes < 0) {
//     perror("recvfrom");
//     return 1;
//   }
//   buffer[nbytes] = '\0';
//   fprintf(stderr, "recvfrom: %d Bytes %s\n", nbytes, buffer);

  // Socket schliessen und Ende
  close(sock_fd);
  return 0;
}