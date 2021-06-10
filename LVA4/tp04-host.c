//
// tp04-host.c
// Einfaches host-Programm
// Hostnamen aus der Kommandozeile -> IP-Adressen
// klin, 25.10.2020

#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// main
int main(int argc, char **argv)
{
    struct hostent *hostptr;
    struct in_addr *hostaddr;
    char *hostname, *aliasname;
    int i;

    // Kommandozeile abarbeiten
    for (i = 1; i < argc; i++)
    {

        // Hostname aus Kommandozeilenparameter
        hostname = argv[i];
        printf("#%d -> %s\n", i, hostname);
        printf("------------------------------------------------\n");

        // hostent-Daten lesen
        if ((hostptr = gethostbyname(hostname)) == NULL)
        {
            fprintf(stderr, "gethostbyname: Host %s nicht gefunden\n\n", hostname);
            continue;
        }

        // Offiziellen Hostnamen ausgeben (A-Rekord)
        printf("Hostname:   %s\n", hostptr->h_name);
        printf("IP-Adresse: %s\n", inet_ntoa(*(struct in_addr *)hostptr->h_addr));

        // Aliase ausgeben
        while ((aliasname = *(hostptr->h_aliases)) != NULL)
        {
            printf("Aliasname:  %s\n", aliasname);
            hostptr->h_aliases++;
        }

        // Adresstyp, Laenge  und IP-Adresse ausgeben
        switch (hostptr->h_addrtype)
        {

        // IPv4
        case AF_INET:
            printf("Adresstyp:  %d (AF_INET), Adresslaenge: %d Bytes\n",
                   hostptr->h_addrtype, hostptr->h_length);
            // IP-Adressen ermitteln und ausgeben
            while ((hostaddr = (struct in_addr *)*hostptr->h_addr_list) != NULL)
            {
                printf("IP-Adresse: %s\n", inet_ntoa(*hostaddr));
                hostptr->h_addr_list++;
            }
            break;

        // IPv6
        case AF_INET6:
        // sonstige
        default:
            fprintf(stderr, "Adresstyp %d unbekannt\n\n", hostptr->h_addrtype);
            break;
        }
        printf("\n");

    } // for(...)

    return 0;
}