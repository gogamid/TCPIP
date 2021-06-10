//
// p4-unclient.c
// Uebung 4 - einfacher UNIX-Socket-Client
// schickt Nachricht an Server und gibt Antwort aus.
// Socketname wird ueber Kommandozeile uebergegeben.
// klin, 10.12.2017

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

// main
int main(int argc, char **argv)
{
  struct sockaddr_un server_addr;
  int sock_fd;
  char *progname;
  char *sockname;
  char buffer[1024];
  int nbytes, rval, length;

  // Programmnamen zuweisen
  progname = argv[0];

  // Sockenamen aus Kommandozeilenparametern
  if(argc != 2) {  
    fprintf(stderr, "Aufruf: %s socketname\n", progname);
    return 1;  
  }
  sockname = argv[1];
	  
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
  strcpy(server_addr.sun_path, sockname);
  
  // Verbindung zum Server herstellen
  rval = connect(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
  if(rval < 0) {
    fprintf(stderr, "%s: kann Verbindung zum Server nicht herstellen\n", progname);
    return 1;
  }

  // Nachricht bauen
  sprintf(buffer, "Hallo Server. Gruss vom Prozess %d", getpid());
  length = strlen(buffer);
  printf("%s: Nachricht an Server:  [%s], Laenge: %d Bytes\n", progname, buffer, length);
  
  // Nachricht (String) an Server schicken
  length = strlen(buffer);
  nbytes = write(sock_fd, buffer, length);    
  if(nbytes != length) {
    fprintf(stderr, "%s: Fehler beim Schreiben an Server\n", progname);
    return 1;
  }

  // Nachricht (String) von Server lesen und ausgeben
  nbytes = read(sock_fd, buffer, sizeof(buffer) - 1);    
  if(nbytes < 0) {
    fprintf(stderr, "%s: Fehler beim Schreiben an Server\n", progname);
    return 1;
  }
  buffer[nbytes] = '\0';
  printf("%s: Nachricht vom Server [%s], Laenge: %d Bytes\n", progname, buffer, nbytes);
    
  // Socket schliessen
  close(sock_fd);

  return 0;
}