// server

/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

// Adresse, Port und Request
#define SERVER_PORT 8080
#define CONTENT_BUFFER 1000000

// main
int main(int argc, char **argv)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[CONTENT_BUFFER];
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
        char buf[CONTENT_BUFFER];
        char message[CONTENT_BUFFER];
        int nb, rv;
        nb = read(client_fd, buf, sizeof(buf));
        buf[nb] = '\0';
        if (buf[0] == 'a') //cd
        {
            /*changing the directory*/
            char s[100];
            if (chdir(buf + 1) == 0)
                sprintf(message, "directory changed to %s\n", getcwd(s, 100));
            else
                sprintf(message, "error changing directory to %s\n", buf + 1);
        }
        else if (buf[0] == 'b') //get
        {
            char content_buffer[CONTENT_BUFFER];
            int fdr, nr;
            fdr = open(buf + 5, O_RDONLY);
            if (fdr == -1)
            {
                fprintf(stderr, "file called %s does not exist\n", buf + 5);
                return 1;
            }
            nr = read(fdr, content_buffer, sizeof(content_buffer));
            sprintf(message, "%s", content_buffer);
        }
        else if (buf[0] == 'c') //put
        {
            // get the file name
            char str[CONTENT_BUFFER];
            sprintf(str, "%s", buf);

            int init_size = strlen(str);
            char delim[] = " ";

            char *ptr = strtok(str, delim);
            ptr = strtok(NULL, delim);

            //get the message
            sprintf(buf, "%s", buf + (3 + strlen(ptr)));

            char fileName[100];
            sprintf(fileName, "%s", ptr);
            int fdw, nw;
            fdw = open(fileName, O_WRONLY | O_CREAT, 0644);
            if (fdw == -1)
                printf("fehler beim oefnen des files");
            nw = write(fdw, buf, nb);
            sprintf(message,"file is copied from client with name %s\n", fileName);
        }
        else
        {

            /* get the command result */
            FILE *fp = NULL;
            char path[1035];

            /* Open the command for reading. */
            sprintf(buf, "%s 2>&1", buf); //2>&1 means write stderr (2) to stdout (1) but it doesn't drop stdout
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
            /* close */
            pclose(fp);
            //////////command result saved to message////////
        }

        message[strlen(message)] = '\0';

        // Nachricht erstellen
        sprintf(buffer, "%s", message);
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
