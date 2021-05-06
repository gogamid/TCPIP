// tcp_client.c
// klin, 05.09.2020
// Einfacher TCP-Client, liest Zufallsdaten vom Server
// Achtung: Keine Fehlerbehandlung!
// Aufruf:  tcp_client server_ip [n_cycles]

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Port, default Zyklen und maximale Computezeit
const int srv_port =  8080;
const int n_cycles = 8;
const int comp_time = 3;

// Main
int main(int argc, char **argv)
{
  struct sockaddr_in server_addr;       // Serveradresse
  char *server_ip;                      // Server IP
  int sock_fd;                          // Socket
  int nc, nb, rv, c, d, t;

  // Kommandozeilenparameter bearbeiten
  if(argc < 2) {
    fprintf(stderr, "Aufruf: %s Server-IP-Adresse [N-Zyklen]\n", argv[0]);
    exit(1);
  }
  // 1. Parameter -> IP-Adresse des Servers
  server_ip = argv[1];
  // 2. Parameter -> Anzahl Zyklen
  if(argc > 2)
    nc = atoi(argv[2]);
  if(nc <= 0)
    nc = n_cycles;

  // -------- Setup / Einrichtung des Socket
  
  // Socket oeffnen
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  fprintf(stderr, "CLIENT SOCKET %d\n", sock_fd);

  // Serveradresse einstellen
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(srv_port);
  
  // Zum Server verbinden
  rv = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  fprintf(stderr, "CLIENT CONNECT %d / running %d cycles\n", rv, nc);

  // Zufallszahlengenerator fuettern
  srand(time(NULL));

  // -------- Protokoll- / Kommunikationsschleife
  
  for(c = 1; c <= nc; c++) {

    // Auf Datum vom Server warten und lesen  
    fprintf(stderr, "CLIENT: cycle %d / waiting for data\n", c);
    nb = read(sock_fd, &d, sizeof(d));
    d = ntohl(d);
    fprintf(stderr, "CLIENT: cycle %d / read %d bytes / data %d\n", c, nb, d);
     
    // Rechenzeit und computen ...      
    t = rand() % comp_time;
    fprintf(stderr, "CLIENT: cycle %d / computing time %d\n", c, t);
    sleep(t);
    
    // Als Bestaetigung Anzahl gelesener Bytes senden
    fprintf(stderr, "CLIENT: cycle %d / sending acknowledge\n", c);
    nb = htonl(nb);
    nb = write(sock_fd, &nb, sizeof(nb));
  }

  // Verbindung und Socket schliessen und kurze Pause
  close(sock_fd);
  sleep(1);
  fprintf(stderr, "CLIENT: terminates after %d cycles\n", nc);
  return 0;

}