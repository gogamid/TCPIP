/*
UDP read write
Der Broker speichert die Nachrichten nicht,sondern leitet eine Nachricht nur weiter, wenn der topic von einem
Subscriber abonniert wurde

Der Broker soll Nachrichten vom Publisher empfangen und an den Subscriber
weiterleiten. Dazu muss smbbroker ständig im Vorder- oder im Hintergrund
laufen und soll eingehende Publish- und Subscribeanforderungen auf der
Konsole ausgeben.

kennt keine topics, kennt nur struktur


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

struct node
{
    int data;       //port number
    char key[20];   //topic name
    struct node *next;
};
struct node *head = NULL;
struct node *current = NULL;

//insert link at the first location
void insertFirst(char key[20], int data)
{
    //create a link
    struct node *link = (struct node *)malloc(sizeof(struct node));

    sprintf(link->key, "%s", key);
    link->data = data;

    //point it to old first node
    link->next = head;

    //point first to new first node
    head = link;
}

//find a link with given key
int find(char key[20])
{
    //start from the first link
    struct node *current = head;

    //if list is empty
    if (head == NULL)
    {
        return 0;
    }

    //navigate through list
    while (strcmp(current->key, key) != 0)
    {

        //if it is last node
        if (current->next == NULL)
        {
            return 0;
        }
        else
        {
            //go to next link
            current = current->next;
        }
    }

    //if data found, return the current Link
    return current->data;
}

// Port
const int srv_port = 8080;

// main
int main(int argc, char **argv)
{
    int server_fd;                               // Socket
    struct sockaddr_in server_addr, client_addr; // Server- und Clientadressen
    socklen_t server_size, client_size;          // Adresslaengen
    char buffer[512];                            // Schreib-/Lesepuffer
    int nbytes, length;

    // Server Socket anlegen und oeffnen
    // Familie: Internet, Typ: UDP-Socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    // Serverstruktur einrichten
    // Datenstruktur auf 0 setzen
    // Familie: Internetserver
    // Adresse: beliebige Clientadressen zulassen
    // Port: wie in srv_port festgelegt
    server_size = sizeof(server_addr);
    memset((void *)&server_addr, 0, server_size);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(srv_port);
    bind(server_fd, (struct sockaddr *)&server_addr, server_size);

    printf("Broker is waiting for messages....\n");
    // In Endlosschleife auf Nachrichten von Clients warten
    while (1)
    {
        // Auf Eingangsnachricht warten und diese lesen
        client_size = sizeof(client_addr);
        nbytes = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_size);
        buffer[nbytes] = '\0';

        printf("\nmessage---%s---is received from publisher\n", buffer);

        // //save the port of subscrieber
        // in_port_t portN = client_addr.sin_port;
        if (buffer[0] == 'p')
        {
            sprintf(buffer, "%s", buffer + 1);

            //parsing string
            //saving topic to ptr
            char str[20];
            sprintf(str, "%s", buffer);
            char delim[] = " ";
            char *ptr = strtok(str, delim);

            //find topic from list
            int port = find(ptr);
            if (port)
            {
                //send to subscriber
                struct sockaddr_in test_addr;
                test_addr.sin_family = AF_INET;
                test_addr.sin_addr = client_addr.sin_addr;
                test_addr.sin_port = port;
                socklen_t test_size = sizeof(test_addr);

                // Antwortnachricht erstellen
                length = strlen(buffer);
                // Nachricht an Client senden
                nbytes = sendto(server_fd, buffer, length, 0, (struct sockaddr *)&test_addr, test_size);
                if (nbytes != length)
                {
                    perror("sendto");
                    return 1;
                }
                printf("\nmessage---%s---is sent to subscriber\n", buffer);
            }

            else
            {
                printf("\n***Topic %s does not have a subscriber***\n", ptr);
            }
        }
        else if (buffer[0] == 's')
        {
            sprintf(buffer, "%s", buffer + 1);
            int port = find(buffer);
            if (port)
                printf("\n***Topic has already subscriber***\n");
            else
            {
                in_port_t portN = client_addr.sin_port;
                insertFirst(buffer, portN);
                printf("\n***Topic %s is subscribed***\n", buffer);
            }
        }
    }

    return 0;
}