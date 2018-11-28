#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct midco
{
    char op[10];	//操作符
	char scr1[128];	//第一个操作数
	char scr2[64];	//第二个操作数
    char dst[64];
}midco;
extern midco midcode[1000];
extern int midconum;

extern FILE *fdag;
void combine()
{
    int i = 0,num1,num2;
    char cal[100];
    for(i = 0;i<midconum;i++)
    {
        if(strcmp(midcode[i].op,"+")==0 || strcmp(midcode[i].op,"-")==0 ||
           strcmp(midcode[i].op,"*")==0 || strcmp(midcode[i].op,"/")==0)
        {
            if(isdigit(midcode[i].scr1[0])!=0 || midcode[i].scr1[0]=='-')
            {
                num1 = atoi(midcode[i].scr1);
            }
            else
            {
                fprintf(fdag,"%s, %s, %s, %s\n",midcode[i].op,midcode[i].scr1,midcode[i].scr2,midcode[i].dst);
                continue;
            }
            if(isdigit(midcode[i].scr2[0])!=0 || midcode[i].scr2[0]=='-')
            {
                num2 = atoi(midcode[i].scr2);
            }
            else
            {
                fprintf(fdag,"%s, %s, %s, %s\n",midcode[i].op,midcode[i].scr1,midcode[i].scr2,midcode[i].dst);
                continue;
            }
            if(strcmp(midcode[i].op,"+")==0)
                itoa(num1+num2,cal,10);
            else if(strcmp(midcode[i].op,"-")==0)
                itoa(num1-num2,cal,10);
            else if(strcmp(midcode[i].op,"*")==0)
                itoa(num1*num2,cal,10);
            else
                itoa((int)(num1/num2),cal,10);
            strcpy(midcode[i].scr1,cal);
            strcpy(midcode[i].op,"ass");
            strcpy(midcode[i].scr2,"\0");
        }
        fprintf(fdag,"%s, %s, %s, %s\n",midcode[i].op,midcode[i].scr1,midcode[i].scr2,midcode[i].dst);
    }

}
