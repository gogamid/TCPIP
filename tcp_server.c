// tcp_server.c
// klin, 05.09.2020
// Einfacher TCP-Server, schickt Zufallsdaten an Client
// Achtung: Keine Fehlerbehandlung!
// Aufruf:  tcp_server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //FUER PROZESSE
#include <string.h>
#include <time.h>
#include <sys/types.h> //FER PROZESSE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Port und maximale Computezeit
const int srv_port = 8080;
const int comp_t = 3;

// Server- und Clientsockets 
int srvsock_fd = -1;
int clisock_fd = -1;
// Verbindungszaehler
int connect_no = 0;

// Main
int main()
{
  struct sockaddr_in server_addr;       // Serveradresse
  struct sockaddr_in client_addr;       // Clientadresse
  socklen_t client_len;                 // Adresslaenge
  int rv;

  // -------- Setup / Einrichtung des Serversocket

  // Serversocket oeffnen
  srvsock_fd = socket(AF_INET, SOCK_STREAM, 0);
  fprintf(stderr, "SERVER: SOCKET %d\n", srvsock_fd);

  // Serveradresse einstellen
  memset((char *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(srv_port);
  
  // Serversocket an Adresse binden
  rv = bind(srvsock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  fprintf(stderr, "SERVER: BIND %d\n", rv);

  // Socket auf Empfangsbereitschaft einstellen
  rv = listen(srvsock_fd, 5);
  fprintf(stderr, "SERVER: LISTEN %d on PORT %d\n", rv, srv_port);

  // -------- Kommunikationschleife
  
  while(1) {
    int nb, c, d, t;

    // Auf Verbindungsanforderung von Client warten
    fprintf(stderr, "SERVER ACCEPTING connection ... ");
    fflush(stderr);
    client_len = sizeof(client_addr);
    clisock_fd = accept(srvsock_fd, (struct sockaddr *) &client_addr, &client_len);
    fprintf(stderr, "\n");
    fprintf(stderr, "SERVER CLIENT SOCKET %d\n", clisock_fd);

    // Verbindungszaehler erhoehen, Zufallszahlengenerator fuettern
    connect_no++;
    srand(time(NULL));
    
    // Protokollschleife: Datum an Client senden und auf Bestaetigung warten
    c = 1;
    while(1) {
    
      // Zufallszahl 0..9999 und Rechenzeit erzeugen
      d = rand() % 9999;
      t = 1 + rand() % comp_t;
      fprintf(stderr, "SERVER: cycle %d / compute time %d\n", c, t);
      // Jetzt computen ...
      sleep(t);

      // Datum (in Netzformat) an Client senden      
      fprintf(stderr, "SERVER: cycle %d / write data %d\n", c, d);
      d = htonl(d);
      nb = write(clisock_fd, &d, sizeof(d));
      // Schreiben erfolglos -> Ende oder Fehler -> Schleife verlassen
      if(nb <= 0) {
	fprintf(stderr, "SERVER: cycle %d / end/error in write to client\n", c);
	break;
      }
      fprintf(stderr, "SERVER: cycle %d / written %d bytes\n", c, nb);

      // Auf Bestaetigung von Client warten
      fprintf(stderr, "SERVER: cycle %d / waiting for acknowledge\n", c);
      nb = read(clisock_fd, &d, sizeof(d));
      // Lesen erfolglos -> Ende oder Fehler -> Schleife verlassen
      if(nb <= 0) {
	fprintf(stderr, "SERVER: cycle %d / end/error in read from client\n", c);
	break;
      }
      d = ntohl(d);
      fprintf(stderr, "SERVER: cycle %d / handshake read %d/%d\n", c, d, nb);
      c++;
    }

    // Clientverbindung / -socket schliessen 
    fprintf(stderr, "SERVER: close client connection/socket %d after %d cycles\n", clisock_fd, --c);
    close(clisock_fd);
    clisock_fd = -1;
  }

  // Serversocket schliessen und Ende
  close(srvsock_fd);
  fprintf(stderr, "SERVER: terminates\n");
  return 0;
}