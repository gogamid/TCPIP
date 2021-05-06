//
// p3-client.c
// Uebung 3 - einfacher TCP-Client, schickt N-mal einen Zaehler 
// an den Server, liest die Serverantwort und gibt diese aus
// Server: maya.rz.hs-fulda.de - 193.174.30.140
// URL: maya.rz.hs-fulda.de/tp-count.php
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

// 5 Verbindungen, Wartezeit 10 Sekunden
#define NCONNS  5
#define DELAY   10

// Adresse, Port und Request
#define SERVER_IPADDR   "193.174.30.140"
#define SERVER_PORT     80
// Der String fuer den Serverrequest wird als Formatstring fuer sprintf()
// genutzt -> der Zaehler im URL '?c=%d' ist als Platzhalter fuer die 
// Integerzaehlervariable formuliert
#define SERVER_REQUEST  "GET /tp-count.php?c=%d HTTP/1.0\r\n\r\n"

// main
int main(int argc, char **argv)
{
  int sock_fd;
  struct sockaddr_in server_addr;
  char buffer[512];
  int nbytes, rval, length;
  int count;

  // Protokoll: 
  // 1. Server wartet auf Verbindung
  // 2. Client baut Verbindung zum Server auf
  // 3. Client schickt Request an Server
  // 4. Server verarbeitet Request
  // 5. Server schickt Antwort an Client
  // 6. Server wartet auf die naechste Verbindung -> Schritt 1
  // -> Client muss in jeder Runde neue Verbindung aufbauen,
  //    d.h. Socket kreieren usw.  
  for(count = 1; count <= NCONNS; count++) {
    
    // Socket anlegen und oeffnen
    // Familie: Internet, Typ: TCP-Socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    fprintf(stderr, "socket: %d\n", sock_fd);
    if(sock_fd < 0) {
      perror("socket");
      return 1;
    }
  
    // Serverstruktur einrichten 
    // Datenstruktur auf 0 setzen
    // Familie: Internetserver
    // Adresse: maya.rz.hs-fulda.de
    // Port: HTTP-Port 80
    memset((void *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IPADDR);
    server_addr.sin_port = htons(SERVER_PORT);
  
    // Zum Server verbinden  
    rval = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    fprintf(stderr, "connect: %d\n", rval);
    if(rval < 0) {
      perror("connect");
      return 1;
    }

    // Request  an Server schicken
    // SERVER_REQUEST ist Formatstring mit Platzhalter fuer Zaehler (s.o.)
    sprintf(buffer, SERVER_REQUEST, count);
    length = strlen(buffer);
    fprintf(stderr, "request: %s len:%d\n", buffer, length);
    nbytes = write(sock_fd, buffer, length);  
    if(nbytes != length) {
      perror("write");
      return 1;  
    }
  
    // Antwort vom Server lesen
    nbytes = read(sock_fd, buffer, sizeof(buffer));
    if(nbytes < 0) {
      perror("read");
      return 1;
    }
    buffer[nbytes] = '\0';
    fprintf(stderr, "read: %d bytes, data: %s\n", nbytes, buffer);

    // Socket schliessen und einen Moment warten
    close(sock_fd);
    sleep(DELAY);
    
  }     // for(count ...
    
  return 0;
}