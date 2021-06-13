
/*
This is server program. It accepts commands from client 
and sends result of that command back to client.

Instruction:
run this in bash before using:

gcc srsmond.c -o srsmond
sudo cp srsmond /usr/local/bin

Usage:
srsmond

@gogamid aka Imron Gamidli
https://github.com/gogamid/TCPIP/tree/main/HA1
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

// Adresse, Port und Request
#define SERVER_PORT 8080

// main
int main(int argc, char **argv)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1000000];
    int nbytes, rval, length;

    // Server Socket anlegen und oeffnen
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    //bind
    memset((void *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Empfangsbereitschaft herstellen und auf Verbindung warten
    listen(server_fd, 5);

    printf("WAITING FOR A CLIENT\n");
    while (1)
    {
        // Verbindungswunsch annehmen
        length = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &length);
        if (client_fd < 0)
        {
            perror("accept");
            break;
        }

        //lesen vom Client
        char buf[10];
        int nb, rv;
        nb = read(client_fd, buf, sizeof(buf));
        buf[nb] = '\0';

        ////// get the command result /////
        FILE *fp = NULL;
        char path[1035];
        char message[1000000];
        int len;
        char ch;

        /* Open the command for reading. */
        fp = popen(buf, "r");
        if (fp == NULL)
        {
            printf("Failed to run command\n");
            exit(1);
        }

        /* Read the output a line at a time - output it. */
        while (fgets(path, sizeof(path), fp) != NULL)
        {
            strncat(message, path, sizeof(path));
        }
        len = strlen(message);
        message[len] = '\0';

        /* close */
        pclose(fp);
        //////////command result saved to message////////

        // Nachricht erstellen
        sprintf(buffer, "%s", message);
        printf("This massage was sent to the client:\n%s\n", message);
        length = strlen(buffer);

        // Nachricht senden
        nbytes = write(client_fd, buffer, length);
        if (nbytes != length)
        {
            perror("write");
            return 1;
        }

        // Kurz warten vor dem Schliessen des Socket
        sleep(2);
        close(client_fd);
        //clean previous message
        memset(message, 0, strlen(message));
    }

    return 0;
}
