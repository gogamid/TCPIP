#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

// Port
const int srv_port = 8080;

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "remotehost and command is needed\nExample: srsmon localhost users\n");
        return 1;
    }
    struct hostent *hostptr;

    // hostent-Daten lesen
    if ((hostptr = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "gethostbyname: Host %s nicht gefunden\n\n", argv[1]);
        return 1;
    }

    char command[10];
    if (strcmp(argv[2], "users") == 0)
        strcpy(command, "who");
    else if (strcmp(argv[2], "procs") == 0)
        strcpy(command, "ps -ef");
    else if (strcmp(argv[2], "nets") == 0)
        strcpy(command, "netstat");
    else if (strcmp(argv[2], "disks") == 0)
        strcpy(command, "df -h");
    else
        strcpy(command, argv[2]);

    if (strcmp(command, argv[2]) == 0)
    {
        fprintf(stderr, "Command %s is not available\n\n", argv[2]);
        return 1;
    }

    struct sockaddr_in server_addr; // Serveradresse
    int sock_fd;                    // Socket
    int rv;

    char *server_ip = inet_ntoa(*(struct in_addr *)hostptr->h_addr);

    // Socket oeffnen
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Serveradresse einstellen
    memset((void *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(srv_port);

    // Zum Server verbinden
    rv = connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Request  an Server schicken
    // SERVER_REQUEST ist Formatstring mit Platzhalter fuer Zaehler (s.o.)
    char buffer[6];
    int nbytes, rval, length;
    sprintf(buffer, command);
    length = strlen(buffer);
    nbytes = write(sock_fd, buffer, length);
    if (nbytes != length)
    {
        perror("write");
        return 1;
    }

    // String vom Server lesen
    char buf[1000000]; // Lesepuffer
    int nb;

    nb = read(sock_fd, buf, sizeof(buf));
    buf[nb] = '\0';
    fprintf(stderr, "%s", buf);

    // Verbindung und Socket schliessen und kurze Pause
    sleep(1);
    close(sock_fd);

    return 0;
}