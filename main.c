#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int op_onoff = 1;//�Ƿ��Ż�
typedef struct midco
{
    char op[10];	//������
	char scr1[128];	//��һ��������
	char scr2[64];	//�ڶ���������
    char dst[64];
}midco;
extern midco midcode[1000];
extern int midconum;

typedef struct opmidco
{
    char op[10];	//������
	char scr1[128];	//��һ��������
	char scr2[64];	//�ڶ���������
    char dst[64];
}opmidco;
extern opmidco opmidcode[1000];
extern int opmidcodenum;
extern FILE *fdag;

void change_mid()
{
    int i;
    for(i = 0;i<opmidcodenum;i++)
    {
        strcpy(midcode[i].scr1, opmidcode[i].scr1);
        strcpy(midcode[i].scr2, opmidcode[i].scr2);
        strcpy(midcode[i].dst, opmidcode[i].dst);
        strcpy(midcode[i].op, opmidcode[i].op);
        //fprintf(fdag,"%s, %s, %s, %s\n",opmidcode[i].op,opmidcode[i].scr1,opmidcode[i].scr2,opmidcode[i].dst);
    }
    midconum = opmidcodenum;
}

int main()
{
    wordana();
    midcodeinit();
    gramana();
    print_sym();
    if(op_onoff==1)
    {
        dag_opti();
    }
    mid2mips(0);
    if(op_onoff==1)
    {
        change_mid();
        combine();
        mid2mips(1);//�Ż�dag���
        change_mid();
        global_reg();//�Ż�ȫ�ּĴ������
    }
    return 0;
}
