//
// p3-server.c
// Uebung 3 - einfacher TCP-Server, sendet einen String 
// mit einem Zaehler der Verbindungsnummer
// 
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

// Adresse, Port und Request
#define SERVER_PORT     8080
// Der String fuer die Serveranwort wird als Formatstring fuer sprintf()
// genutzt -> enthaet einen Platzhalter fuer den Verbindungszaehler
#define SERVER_MESSAGE  "Hallo Client! Verbindung: #%d"

// main
int main(int argc, char **argv)
{
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[512];
  int nbytes, rval, length;
  int count = 0;
  
  // Server Socket anlegen und oeffnen
  // Familie: Internet, Typ: TCP-Socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  fprintf(stderr, "socket: %d\n", server_fd);
  if(server_fd < 0) {
    perror("socket");
    return 1;
  }
  
  // Serverstruktur einrichten 
  // Datenstruktur auf 0 setzen
  // Familie: Internetserver
  // Adresse: beliebige Clientadressen zulassen
  // Port: wie in SERVER_PORT festgelegt
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVER_PORT);
  bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  
  // Empfangsbereitschaft herstellen und auf Verbindung warten  
  fprintf(stderr, "listen\n");
  listen(server_fd, 5);
  
  // Protokoll: 
  // 1. Server wartet auf Verbindung
  // 2. Client baut Verbindung zum Server auf
  // 3. Client schickt Request an Server
  // 4. Server verarbeitet Request
  // 5. Server schickt Antwort an Client
  // 6. Server wartet auf die naechste Verbindung -> Schritt 1
  // -> Server laeuft in einer Endlosschleife
  while(1) {
    // Verbindungswunsch annehmen
    fprintf(stderr, "accept ... ");
    length = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &length);
    fprintf(stderr, "\nclient socketfd: %d\n", client_fd);
    if(client_fd < 0) {
      perror("accept");
      break;
    }
    // Client IP und Port ausgeben
    fprintf(stderr, "client ip-address: %s port: %d\n", 
	    inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
    
    // Nachricht erstellen
    count++;
    sprintf(buffer, SERVER_MESSAGE, count);
    length = strlen(buffer);
    
    // Nachricht senden
    nbytes = write(client_fd, buffer, length);  
    fprintf(stderr, "write: [%s] len: %d\n", buffer, length);
    if(nbytes != length) {
      perror("write");
      return 1;  
    }
    nbytes = write(client_fd, "\n", 2);
  
    // Kurz warten, Verbindung zum Client schliessen und Ende
    sleep(1);
    fprintf(stderr, "close\n");
    close(client_fd);
  }
  
  return 0;
}
