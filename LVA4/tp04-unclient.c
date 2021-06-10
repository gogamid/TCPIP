//
// tp04-unclient.c
// Einfacher UNIX-Socket-Client - schickt Nachricht an Server
// klin, 25.10.2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/tp-unsocket"

// main
int main(int argc, char **argv)
{
  struct sockaddr_un server_addr;
  int sock_fd;
  char *progname;
  char buffer[1024];
  int nbytes, rval, length;

  // Programmnamen zuweisen
  progname = argv[0];
  
  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: TCP-Socket
  sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if(sock_fd < 0) {
    fprintf(stderr, "%s: Kann Socket nicht oeffnen\n", progname);
    return 1;
  }
  
  // Serverstruktur einrichten 
  memset((void *) &server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, SOCKET_PATH);
  
  // Verbindung zum Server herstellen
  rval = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(rval < 0) {
    fprintf(stderr, "%s: kann Verbindung zum Server nicht herstellen\n", progname);
    return 1;
  }

  // Nachricht bauen
  sprintf(buffer, "Hallo Server. Gruss vom Prozess %d", getpid());
  length = strlen(buffer);
  printf("%s: Nachricht: [%s], Laenge: %d Bytes\n", progname, buffer, length);
  
  // Nachricht (String) an Client senden
  nbytes = write(sock_fd, buffer, strlen(buffer));    
  if(nbytes < 0) {
    fprintf(stderr, "%s: Fehler beim Schreiben an Server\n", progname);
    return 1;
  }
    
  // Socket schliessen
  close(sock_fd);

  return 0;
}
