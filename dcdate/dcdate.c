#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc,char** argv)
{
    time_t     curtime;
    struct tm  *lt;
    char       buf[60];
    FILE* hfile;

    curtime = time(NULL);
    lt = localtime(&curtime);
    strftime(buf, sizeof(buf), "#define STR_VERSION \"BUILD %m%d%Y_%I%M%p\"\n", lt);
    hfile = fopen(argv[1],"w+");
    fwrite(buf,1,strlen(buf),hfile);
    fclose(hfile);
    return 0;
}

