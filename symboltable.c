#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct symbol
{
    char name[50];
    int obj;    //��ʶ�����ࣺ����-0 ����-1 ����-2 ����-3
    int type;   //����-0 �ַ���-1 ������-2 void-3
    int value;  //������ֵ�����������ʱ0-���� 1-�ַ���
    int addr;   //����ջ�з��䴢�浥Ԫ����Ե�ַ
    int para;   //����ĳ��Ȼ��ߺ����Ĳ�������
}symbol;

symbol sym[1000];
int funcnum;    //��ǰ��������
int symnum;    //��ǰ��������
int funcindex[100]; //������
extern error(int pos, int errortype);
FILE *fsym;

typedef struct Wnode
{
    int num;
    int type;
    int linen;
    char value[100];
    struct Wnode *next;
}Wnode;
extern struct Wnode *tmp;

void initsymtab();
int insert_sym(char *Name, int Obj, int Type, int Value, int Addr, int Para);
int search_sym();
void print_sym();

//�ڷ��ű��в���һ������
int search_sym(char *Name, int funcflag)
{
    int i;
    if(funcflag==1)//���ҵ��Ǻ���
    {
        for(i=0;i<funcnum;i++)
        {
            if(strcmp(Name,sym[funcindex[i]].name)==0)
                break;
        }
        if(i==funcnum)
            return -1;
        else
            return funcindex[i];
    }
    else
    {
        for(i=funcindex[funcnum-1]+1;i<symnum;i++)//�ֲ��������β�
        {
            if(strcmp(Name,sym[i].name)==0)
                return i;
        }
        for(i=0; i<funcindex[0];i++)//ȫ�ֱ���
        {
            if(strcmp(Name,sym[i].name)==0)
                return i;
        }
        return -1;
    }
}
//����ű��м���һ�����ţ��ɹ����ر�ţ����ɹ�����0
int insert_sym(char *Name, int Obj, int Type, int Value, int Addr, int Para)
{
    int i;
    /*if(funcnum>0)    //��������Ƿ���ȫ�ֱ������ظ�
    {
        for(i=0;i<funcindex[0];i++)
        {
            if(strcmp(Name,sym[i].name)==0)
            {
                printf("1 %s\n",Name);
                error(-1,7);
                return 0;
            }
        }
    }*/
    if(Obj==2)  //func
    {
        for(i=0;i<funcnum;i++)  //��麯�������Ƿ��ظ�
        {
            if(strcmp(Name,sym[funcindex[i]].name)==0)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,31);
                return 0;
            }
        }
        funcindex[funcnum] = symnum;
        funcnum++;
    }
    else    //�Ǻ���
    {
        if(funcnum==0)
            i = 0;
        else
            i = i=funcindex[funcnum-1];
        for(;i<symnum;i++)   //���������Ƿ�͵�ǰ�����ڵķ��������ظ�
        {
            if(strcmp(Name,sym[i].name)==0)
            {
                printf("wrong function type!\n");
                error(tmp->linen,31);
                return 0;
            }
        }
    }
    strcpy(sym[symnum].name,Name);
    sym[symnum].addr = Addr;
    sym[symnum].obj = Obj;
    sym[symnum].para = Para;
    sym[symnum].type = Type;
    sym[symnum].value = Value;
    symnum++;
    if(symnum>=1000)    //maxsymnum
    {
        printf("sym number has reached the maximum!\n");
    }
    return (symnum-1);   //�ɹ����ر��
}

void initsymtab()   //��ʼ�����ű�
{
    int i;
    funcnum = 0;
    symnum = 0;
    for(i = 0; i < 100; i++)
    {
        funcindex[i] = 0;
    }
}

void print_sym()
{
    fsym=fopen("15231105_sym.txt","w");
    if(fsym==NULL)
    {
        printf("Can't open 15231105_sym.txt!\n");
        return 0;
    }
    int i;
    for(i = 0; i<symnum;i++)
        fprintf(fsym,"%s\t%d\t%d\t%d\t%d\t%d\n",sym[i].name,sym[i].obj,sym[i].type,sym[i].value,sym[i].addr,sym[i].para);
}
