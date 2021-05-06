//
// p3-tp03client.c
// Uebung 3 - einfacher Client fuer tp03-server
// Verbindet sich mit Server und liest Serverstring
// IP-Adresse des Server muss uebergeben werden
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Port
const int srv_port =  8080;

// Main
int main(int argc, char **argv)
{
  struct sockaddr_in server_addr;       // Serveradresse
  char *server_ip;                      // Server IP
  char buf[256];                        // Lesepuffer
  int sock_fd;                          // Socket
  int nb, rv;
  
  // Kommandozeilenparameter bearbeiten
  if(argc < 2) {
    fprintf(stderr, "Aufruf: %s Server-IP-Adresse\n", argv[0]);
    exit(1);
  }
  
  // 1. Parameter -> IP-Adresse des Servers
  server_ip = argv[1];
  
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
  fprintf(stderr, "connect %d\n", rv);
  
  // String vom Server lesen
  fprintf(stderr, "read\n");
  nb = read(sock_fd, buf, sizeof(buf));
  buf[nb] = '\0';
  fprintf(stderr, "read %d bytes / data [%s]\n", nb, buf);
  
  // Verbindung und Socket schliessen und kurze Pause
  sleep(1);
  close(sock_fd);
  
  return 0;
}
