//
// tp03-server.c
// Einfacher TCP-Server, sendet einen String
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

// Adresse, Port und Request
#define SERVER_PORT     8080
#define SERVER_MESSAGE  "\n\nHallo Client!\n\n"

// main
int main(int argc, char **argv)
{
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[512];
  int nbytes, rval, length;

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
  
  while(1) {
    // Verbindungswunsch annehmen
    length = sizeof(client_addr);
    fprintf(stderr, "accept ... ");
    client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &length);
    fprintf(stderr, "\nclient: %d\n", client_fd);
    if(client_fd < 0) {
      perror("accept");
      break;
    }
    // Nachricht erstellen
    sprintf(buffer, "%s", SERVER_MESSAGE);
    length = strlen(buffer);

    // Nachricht senden
    nbytes = write(client_fd, buffer, length);  
    fprintf(stderr, "write: %s len: %d\n", buffer, length);
    if(nbytes != length) {
      perror("write");
      return 1;  
    }

    // Kurz warten vor dem Schliessen des Socket 
    sleep(2);
  
    // Verbindung zum Client schliessen und Ende
    fprintf(stderr, "close\n");
    close(client_fd);
  }
  
  return 0;
}
