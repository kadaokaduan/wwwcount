#ifdef __VMS
#include <stdlib.h>
#include <string.h>
#endif

#include "bigplus.h"


int main(int argc,char **argv)
{
    bc_num
        n1,n2,
        result;

    char
        str[100];

    char
        str2[100];

    bc_num
        num;

    int
        scale;


    if (argc != 3)
    {
        (void) fprintf(stderr,"usage: %s <num1> <num2>\n",argv[0]);
        exit(1);
    }


    (void) strcpy(str,argv[1]);
    (void) strcpy(str2,argv[2]);

    init_numbers();
    init_num(&n1);
    init_num(&n2);
    init_num(&result);

    scale=0;

    str2num(&n1,str,scale);
    str2num(&n2,str2,scale);
    bc_add (n1,n2,&result,scale);
    (void) fprintf(stderr,"%s\n",num2str(result));

    free_num(&n1);
    free_num(&n2);
    free_num(&result);
    exit(1);
}
