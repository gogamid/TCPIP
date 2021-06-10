//
// p4-client.c
// Uebung 4 - einfacher TCP-Client, schickt einen Request an Webserver
// liest die Serverantwort und gibt diese aus
// Server und Request werden in Kommandozeile uebergeben
// p4-client server request
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <string.h>

#define SERVICE_NAME "http"
#define SERVICE_PROT "tcp"

// main
int main(int argc, char **argv)
{
  char *progname;
  char *server_name, *server_requ;
  struct hostent *server_hent;
  struct servent *service_sent;
  struct sockaddr_in server_addr;
  int sock_fd;
  char buffer[1024];
  int nbytes, rval, length;

  // Server und Request aus Kommandozeile
  progname = argv[0];
  if(argc != 3) {
    fprintf(stderr, "Aufruf: %s server request\n", progname);
    return 1;
  }
  server_name = argv[1];  
  server_requ = argv[2];  
  
  // Host-Daten des Server abrufen
  if((server_hent = gethostbyname(server_name)) == NULL) {
    fprintf(stderr, "%s: Server %s nicht gefunden\n", progname, server_name);
    return 1;
  }
  // IP-Adresse des Server ausgeben
  fprintf(stderr, "Server-IP-Adresse: %s\n", inet_ntoa(*(struct in_addr *) server_hent->h_addr));

  // Service-Daten abrufen
  if((service_sent = getservbyname(SERVICE_NAME, SERVICE_PROT)) == NULL) {
    fprintf(stderr, "%s: Service %s/%s nicht gefunden\n", progname, SERVICE_NAME, SERVICE_PROT);
    return 1;
  }
  // Service-Daten des Service ausgeben
  fprintf(stderr, "Port fuer %s/%s %d\n", SERVICE_NAME, SERVICE_PROT, htons(service_sent->s_port));

  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: TCP-Socket
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(sock_fd < 0) {
    fprintf(stderr, "%s: Kann Socket nicht oeffnen\n", progname);
    return 1;
  }
  
  // Serverstruktur einrichten 
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  memcpy((void *) &server_addr.sin_addr.s_addr, (void *) server_hent->h_addr, server_hent->h_length);
  server_addr.sin_port = service_sent->s_port;
  
  // Zum Server verbinden  
  rval = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(rval < 0) {
    fprintf(stderr, "%s: Kann Verbindung zum Server nicht  herstellen", progname);
    return 1;
  }
  
  // Request bauen und an Server schicken
  sprintf(buffer, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", server_requ, server_name);
  length = strlen(buffer);
  fprintf(stderr, "%s: Laenge: %d Request: %s\n", progname, length, buffer);
  nbytes = write(sock_fd, buffer, length);  
  if(nbytes != length) {
    fprintf(stderr, "%s: Fehler beim Schreiben\n", progname);
    return 1;  
  }
  
  // Antwort vom Server lesen
  fprintf(stderr, "=======================================================================\n");
  while((nbytes = read(sock_fd, buffer, sizeof(buffer))) > 0) {
    write(fileno(stdout), buffer, nbytes);
  }
  fprintf(stderr, "=======================================================================\n");

  // Socket schliessen und einen Moment warten
  close(sock_fd);
    
  return 0;
}
