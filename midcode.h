#ifndef MIDCODE_H_INCLUDED
#define MIDCODE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define maxmid 1000
/*=============================中间代码操作符==================================*/
char mspace[] = "\0";
char mconst[] = "const";
char mvar[] = "var";
char mfunc[] = "func";
char mmain[] = "main";
char mint[] = "int";
char mvoid[] = "void";
char mchar[] = "char";
char mass[] = "ass";
char massa[] = "assa";
char massret[] = "assret";
char mpara[] = "para";
char mpush[] = "push";
char mcall[] = "call";
char mret[] = "ret";
char mmret[] = "mainret";
char mRET[] = "RET";
char madd[] = "+";
char msub[] = "-";
char mmul[] = "*";
char mdiv[] = "/";
char mequal[] = "==";
char mnequal[] = "!=";
char mgteq[] = ">=";
char mgt[] = ">";
char mlseq[] = "<=";
char mls[] = "<";
char mbnz[] = "bnz";//满足条件跳转
char mbz[] = "bz";//不满足条件跳转
char mjmp[] = "jmp";
char mlabel[] = "label";
char mlpar[] = "(";
char mrpar[] = ")";
char mzero[] = "0";
char mwrite[] = "ptf";
char mread[] = "scf";


typedef struct midco
{
    char op[10];	//操作符
	char scr1[128];	//第一个操作数
	char scr2[64];	//第二个操作数
    char dst[64];
}midco;

midco midcode[maxmid];
int midconum=0;
int tmpvarn=0, labeln=0;

FILE *mout;

void midcodeinit()
{
    mout=fopen("15231105_midcode.txt","w");
    if(mout==NULL)
    {
        printf("Can't open 15231105_midcode.txt!\n");
        return 0;
    }
}

void genmid(char *op, char *scr1, char *scr2, char *dst)
{
    fprintf(mout, "%s, %s, %s, %s\n",op,scr1,scr2,dst);
    strcpy(midcode[midconum].dst, dst);
    strcpy(midcode[midconum].scr1, scr1);
    strcpy(midcode[midconum].op, op);
    strcpy(midcode[midconum].scr2, scr2);
    midconum++;
}

char *gentmpvar()
{
    char *t;
    int i,num[9],w=0;
    t = (char*)malloc(sizeof(char)*10) ;
    t[0] = '$';
    t[1] = 'm';
    for(i=tmpvarn;i>0;i=i/10)
    {
        num[w++] = i%10 + '0';
    }
    for(i=2,w=w-1;w>=0;i++,w--)
        t[i] = num[w];
    t[i] = 0;
    if(tmpvarn==0)
    {
        t[i]='0';
        t[i+1] = 0;
    }
    tmpvarn++;
    return t;
}

char *genlabel()
{
    char *t;
    int i,num[9],w=0;
    t = (char*)malloc(sizeof(char)*10) ;
    t[0] = 'l';
    t[1] = 'a';
    t[2] = 'b';
    t[3] = 'e';
    t[4] = 'l';
    for(i=labeln;i>0;i=i/10)
    {
        num[w++] = i%10 + '0';
    }
    for(i=5,w=w-1;w>=0;i++,w--)
        t[i] = num[w];
    t[i] = 0;
    if(labeln==0)
    {
        t[i]='0';
        t[i+1] = 0;
    }
    labeln++;
    return t;
}

#endif // MIDCODE_H_INCLUDED
