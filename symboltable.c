#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct symbol
{
    char name[50];
    int obj;    //标识符种类：常量-0 变量-1 函数-2 参量-3
    int type;   //整型-0 字符型-1 数组型-2 void-3
    int value;  //常量的值，如果是数组时0-整型 1-字符型
    int addr;   //运行栈中分配储存单元的相对地址
    int para;   //数组的长度或者函数的参数个数
}symbol;

symbol sym[1000];
int funcnum;    //当前函数数量
int symnum;    //当前符号数量
int funcindex[100]; //函数的
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

//在符号表中查找一个符号
int search_sym(char *Name, int funcflag)
{
    int i;
    if(funcflag==1)//查找的是函数
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
        for(i=funcindex[funcnum-1]+1;i<symnum;i++)//局部变量和形参
        {
            if(strcmp(Name,sym[i].name)==0)
                return i;
        }
        for(i=0; i<funcindex[0];i++)//全局变量
        {
            if(strcmp(Name,sym[i].name)==0)
                return i;
        }
        return -1;
    }
}
//向符号表中加入一个符号，成功返回标号，不成功返回0
int insert_sym(char *Name, int Obj, int Type, int Value, int Addr, int Para)
{
    int i;
    /*if(funcnum>0)    //检查名称是否与全局变量名重复
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
        for(i=0;i<funcnum;i++)  //检查函数名称是否重复
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
    else    //非函数
    {
        if(funcnum==0)
            i = 0;
        else
            i = i=funcindex[funcnum-1];
        for(;i<symnum;i++)   //检查符号名是否和当前函数内的符号名称重复
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
    return (symnum-1);   //成功返回标号
}

void initsymtab()   //初始化符号表
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
