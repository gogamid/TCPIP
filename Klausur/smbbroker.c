/*
UDP read write
Der Broker speichert die Nachrichten nicht,sondern leitet eine Nachricht nur weiter, wenn der topic von einem
Subscriber abonniert wurde

Der Broker soll Nachrichten vom Publisher empfangen und an den Subscriber
weiterleiten. Dazu muss smbbroker ständig im Vorder- oder im Hintergrund
laufen und soll eingehende Publish- und Subscribeanforderungen auf der
Konsole ausgeben.

kennt keine topics, kennt nur struktur


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

// Port
const int srv_port = 8080;

// Formatstring der Servernachricht
#define SERVER_MESSAGE  "Hallo Client #%d!"

// main
int main(int argc, char **argv)
{
  int server_fd;                                // Socket
  struct sockaddr_in server_addr, client_addr;  // Server- und Clientadressen
  socklen_t server_size, client_size;           // Adresslaengen
  char buffer[512];                             // Schreib-/Lesepuffer
  int nbytes, length;
  int count;                                    // Aufrufzaehler

  // Server Socket anlegen und oeffnen
  // Familie: Internet, Typ: UDP-Socket
  server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  fprintf(stderr, "socket: %d\n", server_fd);
  if(server_fd < 0) {
    perror("socket");
    return 1;
  }
  
  // Serverstruktur einrichten 
  // Datenstruktur auf 0 setzen
  // Familie: Internetserver
  // Adresse: beliebige Clientadressen zulassen
  // Port: wie in srv_port festgelegt
  server_size = sizeof(server_addr);
  memset((void *) &server_addr, 0, server_size);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(srv_port);
  bind(server_fd, (struct sockaddr *) &server_addr, server_size);

  // In Endlosschleife auf Nachrichten von Clients warten  
  count = 0;
  while(1) {
  
    // Auf Eingangsnachricht warten und diese lesen
    client_size = sizeof(client_addr);
    fprintf(stderr, "recvfrom ... ");
    nbytes = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &client_size);
    buffer[nbytes] = '\0';
    count++;
    fprintf(stderr, "\nrecvfrom: #%d - %d Bytes %s\n", count, nbytes, buffer);

    // Antwortnachricht erstellen
    sprintf(buffer, SERVER_MESSAGE, count);
    length = strlen(buffer);
    // Nachricht an Client senden
    nbytes = sendto(server_fd, buffer, length, 0, (struct sockaddr *) &client_addr, client_size);  
    fprintf(stderr, "sendto: %d Bytes %s\n", nbytes, buffer);
    if(nbytes != length) {
      perror("sendto");
      return 1;  
    }
  
  }
  
  return 0;
}