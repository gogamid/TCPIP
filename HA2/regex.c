#include <stdio.h>
#include <string.h>

int main()
{
    char buf[]="b filename.txt Hello my name is Imron";

    // get the file name
    char str[100000];
    sprintf(str,"%s", buf );

	int init_size = strlen(str);
	char delim[] = " ";

	char *ptr = strtok(str, delim);
    ptr = strtok(NULL, delim);
       
	
     printf("%s\n", ptr);

    //get the message
    sprintf(buf, "%s", buf+(3+strlen(ptr)));
     printf("%s\n", buf);
	return 0;
}
