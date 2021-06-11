#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    FILE *fpipe;
    char *command = "ls";
    char c = 0;
    int i=0;
    char* res;

    if (0 == (fpipe = (FILE*)popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }

    while (fread(&c, sizeof c, 1, fpipe))
    {
        res[i++]=c;

    }

    printf("%s", res);
    pclose(fpipe);

    return 0;
}