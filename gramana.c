#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "gramanaH.h"
#include "midcode.h"

#define resvnum 12

typedef struct Wnode
{
    int num;
    int type;
    int linen;
    char value[100];
    struct Wnode *next;
}Wnode;
extern struct Wnode *head;
extern void wordana();
char NAME[50],Character[20];
char FUNCNAME[50][50];//连续调用
int funcnamei = 0;
char switchend_lab[128][16];
int switchend_labnum = 0;
int expretype = 0;//调用时的参数类型
char tmpfactor[20], tmpterm[20], tmpexpr[20];//表达式相关参数
int OBJ, TYPE, VALUE, ADDR, PARA;//符号表相关参数
int number;//整数
int ismain = 0;
int writetype = 1,writing = 0; //0-int 1-char
int arraytype[100],arraytypenum = 0;
int iscalling = 0;
int voidret = 0;//void型函数是否产生了return语句
int isarrayoffset = 0;//是否是数组位移的表达式
int funcflag;//函数类型参数 void-3 char-1 int-0

typedef struct symbol
{
    char name[50];
    int obj;    //标识符种类：常量-0 变量-1 函数-2 参量-3
    int type;   //整型-0 字符型-1 数组型-2 void-3
    int value;  //常量的值，如果是数组时0-整型 1-字符型
    int addr;   //运行栈中分配储存单元的相对地址
    int para;   //数组的长度或者函数的参数个数
}symbol;

extern symbol sym[1000];

struct Wnode *tmp;
FILE *fgramout;

void constdecl();
void constdef();
void vardecl();
void vardef();
int integer();
void returnvfuncdef();
void returnfuncdef();
void paratable();
void compoundstatement();
void statement();
void statements();
void ifstatement();
void condition();
void dowhilestatement();
void switchstatement();
void casestatement(char *caseexpr,int chartype);
void callretvstatement();
void callretstatement();
int valueparatable();
void assignstatement();
void readstatement();
void writestatement();
void returnstatement();
void mainstatement();
void expression(char *dst);
void expreinside();
void term();
void factor();
void skip2sc();

int gramana()
{
    fgramout=fopen("15231105_grammar.txt","w");
    if(fgramout==NULL)
    {
        printf("Can't open 15231105_grammar.txt!\n");
        return 0;
    }

    tmp=head;
    ADDR = 0;

    //［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞

    if(tmp->type==CONST)//常量说明
    {
        constdecl();//进入时是const，返回分号之后的单词
    }
    if((tmp->type==INT || tmp->type==CHAR) && tmp->next->next->type!=LPAR)//变量说明
    {
        vardecl();//进入时是int或者char，返回分号之后的单词
    }

    while(tmp->type==INT || tmp->type==CHAR || (tmp->type==VOID && tmp->next->type!=MAIN))
    {
        if(tmp->type==VOID && tmp->next->type!=MAIN)
            returnfuncdef();
        else
            returnvfuncdef();
        if(tmp->type==CONST)
        {
            error(tmp->linen,27);
            constdecl();
        }
    }
    if(tmp!=NULL)
        mainstatement();
    else
        error(tmp->linen,26);
    return 0;
}

void skip2sc()
{
    while(tmp->type != 35)
        tmp = tmp->next;
}
/*--------------------------------------------------子函数 声明部分-----------------------------------------------------*/
//＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
//进去的时候是const，出来的时候是分号后的第一个单词
void constdecl()
{
    OBJ = 0;
    while(tmp->type==CONST)
    {
        constdef();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
        }
        fprintf(fgramout,"This is a const declaration sentence!\n");
        if(tmp->next!=NULL && tmp->type==SEMICOL){
            tmp = tmp->next;
        }
        else
            return;
    }
}

/*＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
                            | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}*/
void constdef()
{
    int c_type=-1,rt;
    int character;
    tmp = tmp->next;
    PARA = 0;
    VALUE = 0;
    switch (tmp->type){
        case INT:{c_type = INT;break;}
        case CHAR:{c_type = CHAR;break;}
        default:{error(tmp->linen,6);skip2sc();return;}
    }
    tmp = tmp->next;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,7);
        skip2sc();
        return;
    }
    strcpy(NAME,tmp->value);
    tmp = tmp->next;
    if(tmp->type!=ASS)//=
    {
        error(tmp->linen,9);
        skip2sc();
        return;
    }
    else
    {
        tmp = tmp->next;
        if(c_type==INT){
            TYPE = 0;
            rt=integer();
            if(rt==0)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,10);
                skip2sc();
                return;
            }
            else
            {
                VALUE=number;
                itoa(VALUE,Character,10);
                genmid(mconst,mint,Character,NAME);
            }
        }
        else if(c_type==CHAR){
            TYPE = 1;
            if(tmp->type!=CHARACTER)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,10);
                skip2sc();
                return;
            }
            else
            {
                VALUE = tmp->value[1];
                character = tmp->value[1];
                itoa(character,Character,10);
                genmid(mconst,mchar,Character, NAME);
            }
        }
        insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,0);
        ADDR++;
        tmp = tmp->next;
    }

    while(tmp->type==COMMA)//{,＜标识符＞＝＜整数＞}
    {
        PARA = 0;
        VALUE = 0;
        tmp = tmp->next;
        if(tmp->type!=IDENTITY)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,7);
            skip2sc();
            return;
        }
        strcpy(NAME,tmp->value);
        tmp = tmp->next;
        if(tmp->type!=ASS)//=
        {
            error(tmp->linen,9);
            skip2sc();
            return;
        }
        tmp = tmp->next;
        if(c_type==INT){
            rt=integer();
            if(rt==0)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,10);
                skip2sc();
                return;
            }
            else
            {
                VALUE = number;
                itoa(VALUE,Character,10);
                genmid(mconst,mint,Character,NAME);
            }
        }
        else if(c_type==CHAR){
            if(tmp->type!=CHARACTER)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,10);
                skip2sc();
                return;
            }
            else
            {
                VALUE = tmp->value[1];
                character = tmp->value[1];
                itoa(character,Character,10);
                genmid(mconst,mchar,Character,NAME);
            }
        }
        insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,0);
        ADDR++;
        tmp = tmp->next;
    }
}

//＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
//进去的时候是int或者char，出来的时候是分号后的第一个单词
void vardecl()
{
    OBJ = 1;
    while((tmp->type==INT || tmp->type==CHAR) && tmp->next->next->type!=LPAR)
    {
        vardef();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
        }
        fprintf(fgramout,"This is a variation declaration sentence!\n");
        if(tmp->next!=NULL&&tmp->type==SEMICOL){
            tmp = tmp->next;
        }
        else
            return;
    }

}
//＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’){,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
void vardef()
{
    int c_type=-1;//int\char
    VALUE = 0;
    PARA = 0;
    switch (tmp->type){
        case INT:{c_type = INT;TYPE = 0;break;}
        case CHAR:{c_type = CHAR;TYPE = 1;break;}
        default:{error(tmp->linen,6);skip2sc();return;}
    }
    tmp = tmp->next;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,7);
        skip2sc();
        return;
    }
    strcpy(NAME,tmp->value);
    tmp = tmp->next;
    if(tmp->type==LSQB)//数组部分处理，结束后是]后的单词
    {
        VALUE = TYPE;
        TYPE = 2;
        tmp = tmp->next;
        if(tmp->type!=NUMBER)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,11);
            skip2sc();
            return;
        }
        strcpy(Character, tmp->value);//四元式的数组长度项
        integer();
        PARA = number;
        tmp = tmp->next;
        if(tmp->type!=RSQB)
        {
            error(tmp->linen,12);
            skip2sc();
        }
        tmp = tmp->next;
    }
    insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,PARA);//加入符号表
    ADDR = ADDR + number;
    if(TYPE == 0)//生成四元式
        genmid(mvar, mint, mspace, NAME);
    else if(TYPE == 1)
        genmid(mvar, mchar, mspace, NAME);
    else if(TYPE == 2 && VALUE == 0)
        genmid(mvar, mint, Character, NAME);
    else
        genmid(mvar, mchar, Character, NAME);

    while(tmp->type==COMMA)//{,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’) }
    {
        PARA = 0;
        VALUE =0;
        TYPE = c_type-1;
        tmp = tmp->next;
        if(tmp->type!=IDENTITY)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,7);
            skip2sc();
            return;
        }
        strcpy(NAME,tmp->value);
        tmp = tmp->next;
        if(tmp->type==LSQB)//数组部分处理，结束后是]后的单词
        {
            VALUE = TYPE;
            TYPE = 2;
            tmp = tmp->next;
            if(tmp->type!=NUMBER)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,11);
                skip2sc();
                return;
            }
            strcpy(Character, tmp->value);//四元式的数组长度项
            integer();
            PARA = number;
            tmp = tmp->next;
            if(tmp->type!=RSQB)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,12);
                skip2sc();
            }
            tmp = tmp->next;
        }
        if(TYPE == 0)//生成四元式
            genmid(mvar, mint, mspace, NAME);
        else if(TYPE == 1)
            genmid(mvar, mchar, mspace, NAME);
        else if(TYPE == 2 && VALUE == 0)
            genmid(mvar, mint, Character, NAME);
        else
            genmid(mvar, mchar, Character, NAME);
        insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,PARA);//加入符号表
        ADDR = ADDR + number;
    }
}

//＜整数＞::= ［＋｜－］＜无符号整数＞｜０,返回的时候tmp在数字
int integer()//0不是整数，1整数
{
    int PM=1,i;
    if(tmp->type==ADD || tmp->type==SUB)
    {
        if(tmp->type==SUB)
            PM=0;

        if(tmp->next->type!=NUMBER&&tmp->next->type!=ZERO)
        {
            if(tmp->next->type!=0&&(tmp->next->type==2&&tmp->next->value!=0))
            {
                printf("error: for word %s, ",tmp->next->value);
                error(tmp->linen,13);
            }

        }
        else
        {
            tmp = tmp->next;
        }
    }
    if(tmp->type==ZERO || tmp->type==NUMBER)
    {
        for(i=0,number=0;i<strlen(tmp->value);i++)
        {
            number = number * 10 + (int)(tmp->value[i] - '0');
        }
        if(PM==0)
            number = 0-number;
        return 1;
    }
    else
        return 0;
}


/*===============================================函数定义====================================================================*/
//＜有返回值函数定义＞  ::=  ＜声明头部＞‘(’＜参数表＞‘)’ ‘{’＜复合语句＞‘}’
//＜声明头部＞   ::=  int＜标识符＞|char＜标识符＞
void returnvfuncdef()
{
    int func_type=-1,funcn;//0-char;1-int
    ADDR = 0;
    OBJ = 2;
    VALUE = 0;
    PARA = 0;
    switch (tmp->type){
        case INT:{func_type = INT;TYPE = 0;funcflag = 0;break;}
        case CHAR:{func_type = CHAR;TYPE = 1;funcflag = 1;break;}
        default:{printf("error: for word %s, ",tmp->value);error(tmp->linen,6);}
    }
    tmp = tmp->next;
    if(tmp->type!=IDENTITY)
    {
        error(tmp->linen,7);
        skip2sc();
        return;
    }
    strcpy(NAME,tmp->value);
    //四元式
    if(func_type == INT)
        genmid(mfunc, mint, mspace, NAME);
    else
        genmid(mfunc, mchar, mspace, NAME);

    funcn = insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,PARA);//函数添加符号表
    ADDR++;
    tmp = tmp->next;
    if(tmp->type!=LPAR){//(
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//进入参数表
    paratable();//下一个是）

    sym[funcn].para = PARA;//函数符号表中参量数量的修改
    if(tmp->type!=RPAR){//)
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    if(tmp->type!=LBRACE){//{
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    //复合语句的第一个单词
    compoundstatement();

    if(tmp->type!=RBRACE){//}
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is a definition sentence of return with value function!\n");
}

//＜无返回值函数定义＞  ::= void＜标识符＞‘(’＜参数表＞‘)’‘{’＜复合语句＞‘}’
void returnfuncdef()
{
    int funcn;
    ADDR = 0;
    OBJ = 2;
    TYPE = 3;
    VALUE = 0;
    PARA = 0;
    funcflag = 3;
    voidret = 0;
    if(tmp->type!=VOID)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,30);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,7);
        skip2sc();
        return;
    }
    strcpy(NAME,tmp->value);
    genmid(mfunc, mvoid, mspace, NAME);
    funcn = insert_sym(NAME,OBJ,TYPE,VALUE,ADDR,PARA);//函数名添加符号表
    ADDR++;
    tmp = tmp->next;
    if(tmp->type!=LPAR){//(
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//进入参数表
    paratable();//下一个是）
    sym[funcn].para = PARA;//更改符号表中函数的参量数量
    if(tmp->type!=RPAR){//)
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    if(tmp->type!=LBRACE){//{
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    //复合语句的第一个单词
    compoundstatement();
    genmid(mret,mspace,mspace,mspace);

    if(tmp->type!=RBRACE){//}
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is a definition sentence of return without value function!\n");
}

//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
//出来的时候是最后一个标识符后面的单词，即括号
void paratable()
{
    OBJ = 3;
    VALUE = 0;
    PARA = 0;
    int i_type;
    if(tmp->type==RPAR)
    {
        return;//下一个
    }
    switch (tmp->type){
        case INT:{i_type = INT;TYPE = 0;break;}
        case CHAR:{i_type = CHAR;TYPE = 1;break;}
        default:
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,6);
                skip2sc();
                return;
            }
    }tmp = tmp->next;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,7);
        skip2sc();
        return;
    }

    strcpy(NAME,tmp->value);
    tmp = tmp->next;
    //四元式
    if(i_type == INT)
        genmid(mpara, mint, mspace, NAME);
    else
        genmid(mpara, mchar, mspace, NAME);

    insert_sym(NAME,OBJ,TYPE,0,ADDR,0);//添加符号表
    ADDR++; PARA++;
    while(tmp->type==COMMA)//{,＜标识符＞＝＜整数＞}
    {
        tmp = tmp->next;
        switch (tmp->type){
            case INT:{i_type = INT;TYPE = 0;break;}
            case CHAR:{i_type = CHAR;TYPE = 1;break;}
            default:
                {
                    printf("error: for word %s, ",tmp->value);
                    error(tmp->linen,6);
                    skip2sc();
                    return;
                }
        }tmp = tmp->next;
        if(tmp->type!=IDENTITY)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,7);
            skip2sc();
            return;
        }
        strcpy(NAME,tmp->value);
        tmp = tmp->next;
        //四元式
        if(i_type == INT)
            genmid(mpara, mint, mspace, NAME);
        else
            genmid(mpara, mchar, mspace, NAME);
        insert_sym(NAME,OBJ,TYPE,0,ADDR,0);//符号表
        ADDR++; PARA++;
    }
}

/*=============================================语句===================================================*/
//＜复合语句＞ ::= ［＜常量说明＞］［＜变量说明＞］＜语句列＞
//进来的时候是复合语句的第一个单词，出去的时候是语句列结束后的第一个单词
void compoundstatement()
{
    if(tmp->type==CONST)//常量说明
    {
        constdecl();//进入时是const，返回分号之后的单词
    }
    if(tmp->type==INT || tmp->type==CHAR)//变量说明
    {
        vardecl();//进入时是int或者char，返回分号之后的单词
    }
    statements();
}

//＜语句列＞ ::=｛＜语句＞｝
void statements()
{
    while(tmp->type==IF || tmp->type==DO || tmp->type==SWITCH || tmp->type==SEMICOL ||
          tmp->type==IDENTITY || tmp->type==SCANF || tmp->type==PRINTF ||
          tmp->type==LBRACE || tmp->type==RETURN)
        statement();
    if(tmp->type!=RBRACE)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,29);
        skip2sc();
        return;
        }
}

/*＜语句＞    ::= ＜条件语句＞｜＜循环语句＞｜<情况语句>|‘{’＜语句列＞‘}’
｜＜有返回值函数调用语句＞; |＜无返回值函数调用语句＞;｜＜赋值语句＞;
｜＜读语句＞;｜＜写语句＞;｜＜空＞;｜＜返回语句＞;*/
void statement()
{
    int symnum;
    if(tmp->type==IF)//条件语句
    {
        ifstatement();
    }
    else if(tmp->type==DO)//循环语句
    {
        dowhilestatement();
    }
    else if(tmp->type==SWITCH)//情况语句
    {
        switchstatement();
    }
    else if(tmp->type==LBRACE)//语句列
    {
        tmp = tmp->next;
        statements();
        if(tmp->type!=RBRACE)
        {
            error(tmp->linen,15);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==IDENTITY && tmp->next->type==LPAR)//函数
    {
        symnum = search_sym(tmp->value, 1);
        if(symnum<0)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen, 16);
            skip2sc();
            return;
        }
        else
        {
            funcflag = sym[symnum].type;//设置函数类型
            if(funcflag == 3)
                callretstatement();
            else
                callretvstatement();
        }
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==IDENTITY && (tmp->next->type==ASS || tmp->next->type==LSQB))//赋值语句 填符号表
    {
        assignstatement();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==SCANF)//读语句
    {
        readstatement();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==PRINTF)//写语句
    {
        writestatement();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==RETURN)//返回语句
    {
        returnstatement();
        if(tmp->type!=SEMICOL)
        {
            error(tmp->linen,8);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }
    else if(tmp->type==SEMICOL)//空
    {
        tmp = tmp->next;
    }
    else if(tmp->type!=RBRACE)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,29);
        skip2sc();
        tmp = tmp->next;
    }
    return;
}

//＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞
void ifstatement()
{
    char *label1;
    label1 = (char *)malloc(sizeof(char)*20);
    label1 = genlabel();
    tmp = tmp->next;
    if(tmp->type!=LPAR)//(
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    condition();
    genmid(mbz, mspace, mspace, label1);//不满足条件跳转
    if(tmp->type!=RPAR)//)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    statement();
    genmid(mlabel, mspace, mspace, label1);
    fprintf(fgramout,"This is a if sentence!\n");
}
//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞  返回条件结束后下一个单词
void condition()
{
    int judge;
    int type1,type2;
    char *src1;
    src1 = (char*)malloc(sizeof(char)*20);

    strcpy(tmpexpr,gentmpvar());
    expression(tmpexpr);
    type1 = expretype;
    strcpy(src1, tmpexpr);
    //＜表达式＞＜关系运算符＞＜表达式＞
    if(tmp->type==LEQ || tmp->type==LE || tmp->type==GTQ || tmp->type==GT || tmp->type==EQU || tmp->type==NEQ)
    {
        judge = tmp->type;
        tmp = tmp->next;
        strcpy(tmpexpr,gentmpvar());
        expression(tmpexpr);
        type2 = expretype;
        /*if(type1!=type2)
        {
            error(tmp->num,28);
            skip2sc();
            return;
        }*/
        if(judge == LEQ)
            genmid(mlseq, src1, tmpexpr, mspace);
        else if(judge == LE)
            genmid(mls, src1, tmpexpr, mspace);
        else if(judge == GTQ)
            genmid(mgteq, src1, tmpexpr, mspace);
        else if(judge == GT)
            genmid(mgt, src1, tmpexpr, mspace);
        else if(judge == EQU)
            genmid(mequal, src1, tmpexpr, mspace);
        else if(judge == NEQ)
            genmid(mnequal, src1, tmpexpr, mspace);
        free(src1);
    }
    else//｜＜表达式＞
    {
        genmid(mnequal, src1, mzero, mspace);
        free(src1);
        if(tmp->next->type!=LEQ&&tmp->next->type!=LE&&tmp->next->type!=GTQ
           &&tmp->next->type!=GT&&tmp->next->type!=EQU&&tmp->next->type!=NEQ)
            return;
        else
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,17);
            skip2sc();
            return;
        }
    }
}

//＜循环语句＞ ::=  do＜语句＞while ‘(’＜条件＞‘)’  返回循环语句之后的一句
void dowhilestatement()
{
    char *label1;
    label1 = (char *)malloc(sizeof(char)*20);
    label1 = genlabel();

    if(tmp->type!=DO)
    {
        error(tmp->linen,18);
    }
    else
        tmp = tmp->next;
    genmid(mlabel, mspace, mspace, label1);
    statement();
    if(tmp->type!=WHILE)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,18);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    if(tmp->type!=LPAR)//(
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;

    condition();
    genmid(mbnz, mspace, mspace, label1);//满足条件跳转到语句
    if(tmp->type!=RPAR)//)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is a do-while sentence!\n");
}

//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞ ‘}’
//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞} 返回}后的下一个单词
void switchstatement()
{
    char *caseexpr;
    int chartype;
    caseexpr = (char*)malloc(sizeof(char)*20);
    switchend_labnum++;
    strcpy(switchend_lab[switchend_labnum],genlabel());
    if(tmp->type!=SWITCH)//switch
    {
        error(tmp->linen,18);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//(
    if(tmp->type!=LPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//＜表达式＞


    expression(tmpexpr);
    strcpy(caseexpr, tmpexpr);
    chartype = expretype;

    if(tmp->type!=RPAR)//)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//{
    if(tmp->type!=LBRACE)
    {
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    casestatement(caseexpr,chartype);//＜情况子语句＞
    while(tmp->type==CASE)//{＜情况子语句＞}
        casestatement(caseexpr,chartype);

    genmid(mlabel,mspace,mspace,switchend_lab[switchend_labnum]);
    switchend_labnum--;
    if(tmp->type!=RBRACE)//}
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,29);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is a switch sentence!\n");
}

//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
//＜常量＞   ::=  ＜整数＞|＜字符＞
void casestatement(char *caseexpr,int chartype)
{
    char *caseconst,*label,*CH;
    int charvalue;
    caseconst = (char*)malloc(sizeof(char)*20);
    CH = (char*)malloc(sizeof(char)*20);
    label = (char*)malloc(sizeof(char)*10);
    label = genlabel();

    if(tmp->type!=CASE)//case
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,18);
    }
    tmp = tmp->next;//常量
    if(integer()==0 && tmp->type!=CHARACTER)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,32);
        skip2sc();
        return;
    }

    caseconst = gentmpvar();
    if(tmp->type == CHARACTER)
    {
        if(chartype!=1)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,28);
            skip2sc();
            return;
        }
        charvalue = (int)(tmp->value[1]);
        itoa(charvalue,CH,10);
        genmid(mass,CH,mspace,caseconst);
    }
    else
    {
        if(chartype!=0)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,28);
            skip2sc();
            return;
        }
        genmid(mass,tmp->value,mspace,caseconst);
    }

    genmid(mequal, caseexpr, caseconst, mspace);
    genmid(mbz, mspace, mspace, label);//常量不相等则跳过这个case
    tmp = tmp->next;
    if(tmp->type!=COLON)
    {
        error(tmp->linen,8);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;

    statement();
    genmid(mjmp,mspace,mspace,switchend_lab[switchend_labnum]);
    genmid(mlabel, mspace, mspace, label);//在这个case的语句之后设label
    fprintf(fgramout,"This is a case sentence!\n");
    free(caseconst);
}

//＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
void callretvstatement()
{
    int num, rt,callflag;

    funcnamei++;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,16);
        skip2sc();
        return;
    }
    //检查函数名
    num = search_sym(tmp->value, 1);
    if(num<0)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,16);
        skip2sc();
        return;
    }
    else
    {
        if(sym[num].type == 3)//调用了void型
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,20);
            skip2sc();
            return;
        }
        if(sym[num].type == 0 &&writing == 1)
            writetype = 0;//输出类型
        else if(sym[num].type==1 &&writing ==1)
            writetype = 1;
    }
    if(sym[num].type!=0 && sym[num].type!=1)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen, 20);
        skip2sc();
        return;
    }
    PARA = sym[num].para;
    strcpy(FUNCNAME[funcnamei], tmp->value);

    tmp = tmp->next;//(
    if(tmp->type!=LPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//值参数表
    if(tmp->type==RPAR)//值参数表为空
    {
        genmid(mcall, mspace, mspace, FUNCNAME[funcnamei]);    //call funcname
        funcnamei--;
        strcpy(tmpexpr,gentmpvar());
        genmid(massret, mRET, mspace, tmpexpr);    //i = ret
        tmp = tmp->next;
        fprintf(fgramout,"This is a calling return with value function sentence!\n");
        return;
    }
    rt = valueparatable();
    if(rt!=PARA)//检查参量是否相同
    {
        error(tmp->linen, 21);
        skip2sc();
        return;
    }

    if(tmp->type!=RPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;

    genmid(mcall, mspace, mspace, FUNCNAME[funcnamei]);    //call funcname
    funcnamei--;
    strcpy(tmpexpr,gentmpvar());
    genmid(massret, mRET, mspace, tmpexpr);    //i = ret
    fprintf(fgramout,"This is a calling return with value function sentence!\n");
}

//＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
void callretstatement()
{
    int rt, num, callflag;
    funcnamei++;
    if(tmp->type!=IDENTITY)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,16);
        skip2sc();
        return;
    }

    //检查函数名
    num = search_sym(tmp->value, 1);
    if(num<0)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen, 16);
        skip2sc();
        return;
    }
    else
    {
        callflag = sym[num].type;//设置函数类型
        if(callflag != 3)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen, 20);
            skip2sc();
            return;
        }
    }
    if(sym[num].type!=3)
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen, 20);
        skip2sc();
        return;
    }
    PARA = sym[num].para;
    strcpy(FUNCNAME[funcnamei], tmp->value);

    tmp = tmp->next;//(
    if(tmp->type!=LPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;//值参数表
    if(tmp->type==RPAR)//值参数表为空
    {
        genmid(mcall, mspace, mspace, FUNCNAME[funcnamei]);    //call funcname
        tmp = tmp->next;
        funcnamei--;
        fprintf(fgramout,"This is a calling return with value function sentence!\n");
        return;
    }

    rt = valueparatable();
    if(rt!=PARA)//检查参量是否相同
    {
        error(tmp->linen, 21);
        skip2sc();
        return;
    }

    if(tmp->type!=RPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;

    genmid(mcall, mspace, mspace, FUNCNAME[funcnamei]);    //call funcname
    funcnamei--;
    fprintf(fgramout,"This is a calling return with value function sentence!\n");
}

//＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
int valueparatable()
{
    int num=0,i = 0;
    int paratype = 0;//函数需要的类型
    int callfuncnum;
    char paratable[100][100];
    char paraname[64];

    callfuncnum = search_sym(FUNCNAME[funcnamei],1);
    strcpy(paraname,tmp->value);//保存参量名称，防止出错
    strcpy(tmpexpr,gentmpvar());
    expression(tmpexpr);
    strcpy(paratable[num],tmpexpr);   //get x
    num++;
    paratype = sym[callfuncnum+num].type;
    if(paratype!=expretype)
    {
        printf("error: for word %s, ",paraname);
        error(tmp->linen,33);
        skip2sc();
        return -1;
    }

    while(tmp->type==COMMA)
    {
        tmp = tmp->next;
        strcpy(paraname,tmp->value);//保存参量名称，防止出错
        strcpy(tmpexpr,gentmpvar());
        expression(tmpexpr);
        strcpy(paratable[num],tmpexpr);
        num++;
        paratype = sym[callfuncnum+num].type;
        if(paratype!=expretype)
        {
            printf("error: for word %s, ",paraname);
            error(tmp->linen,33);
            skip2sc();
            return;
        }
    }
    for(i=0;i<num;i++)
        genmid(mpush,mspace,mspace,paratable[i]);
    return num;
}

//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assignstatement()
{
    char array[20];
    char dst[20];
    int arrayflag = 0,arrayoffset;
    int type1,type2,para;
    int i;

    if(tmp->type==IDENTITY)
    {
        strcpy(dst, tmp->value);//标识符=name
        i = search_sym(tmp->value, 0);
        if(i<0)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen, 31);
            skip2sc();
            return;
        }
        if(sym[i].obj!=1&&sym[i].obj!=3)//只能给变量或者参量赋值
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen, 28);
            skip2sc();
            return;
        }
        para = sym[i].para;
        type1 = sym[i].type;
    }
    else{
        error(tmp->linen,6);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    //‘[’＜表达式＞‘]’??
    if(tmp->type!=LSQB)
    {
        if(type1 == 2)
        {
            error(tmp->linen,28);
            skip2sc();
            return;
        }
    }
    else
    {
        arrayflag = 1;
        isarrayoffset = 1;
        arraytypenum++;
        arraytype[arraytypenum] = 1;
        tmp = tmp->next;

        if((tmp->type==14||tmp->type==15)&&tmp->next->type==23&&integer()>0)//判断静态数组,数字之后接右括号
        {
            arrayoffset = atoi(tmp->value);
            if(para<=arrayoffset)
            {
                error(tmp->linen,24);//越上界
                strcpy(tmpexpr,"0");
            }
            if(arrayoffset<0)
            {
                error(tmp->linen,24);//越下界
                strcpy(tmpexpr,"0");
            }
        }

        strcpy(tmpexpr,gentmpvar());
        expression(tmpexpr);
        strcpy(array, tmpexpr);
        if(arraytype[arraytypenum]==1)
        {
            error(tmp->linen,11);
            skip2sc();
            return;
        }
        isarrayoffset = 0;
        arraytypenum--;
        if(tmp->type!=RSQB)
        {
            error(tmp->linen,12);
            skip2sc();
            return;
        }
        else
            tmp = tmp->next;
    }//＝＜表达式＞
    if(tmp->type!=ASS)
    {
        error(tmp->linen,9);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;

    strcpy(tmpexpr,gentmpvar());
    expression(tmpexpr);
    type2 = expretype;
    /*if(type1!=type2&&!(type1==0&&type2==1))
    {
        error(tmp->num,28);
        skip2sc();
        return;
    }*/
    if(arrayflag == 0)//非数组
        genmid(mass,tmpexpr, mspace, dst);//a=tmpexpr
    else
        genmid(massa,tmpexpr, array, dst);//name =
    fprintf(fgramout,"This is an assign sentence!\n");
}

//＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void readstatement()
{
    int symnum, readtype;
    if(tmp->type!=SCANF)
    {
        error(tmp->linen,18);
        skip2sc();
        return;
    }
    tmp = tmp->next;//(
    if(tmp->type!=LPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    tmp = tmp->next;//标识符
    if(tmp->type==IDENTITY)
    {
        if(search_sym(tmp->value, 0)>=0)
            {
                symnum = search_sym(tmp->value, 0);
                readtype = sym[symnum].type;//储存类型 int为0 char为1
                if(readtype == 0)
                genmid(mread, mint, mspace, tmp->value);
            else
                genmid(mread, mchar, mspace, tmp->value);
        }
        else
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,31);
            skip2sc();
            return;
        }
    }
    else
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,22);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    while(tmp->type==COMMA)
    {
        tmp = tmp->next;
        if(tmp->type==IDENTITY)
        {
            if(search_sym(tmp->value, 0)>=0)
            {
                symnum = search_sym(tmp->value, 0);

                readtype = sym[symnum].type;//储存类型 int为0 char为1
                if(readtype == 0)
                    genmid(mread, mint, mspace, tmp->value);
                else
                    genmid(mread, mchar, mspace, tmp->value);
            }
            else
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen,22);
                skip2sc();
                return;
            }
        }
        else
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen,22);
            skip2sc();
            return;
        }
        tmp = tmp->next;
    }
    if(tmp->type!=RPAR)//)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is a read sentence!\n");
}

//＜写语句＞    ::=  printf‘(’＜字符串＞,＜表达式＞‘)’|printf ‘(’＜字符串＞‘)’|printf ‘(’＜表达式＞‘)’
void writestatement()
{
    char *writee, *writes;
    int i, slen, exprflag = 0;
    writing = 1;
    writetype = 1;//先都定义成char型
    writee = (char*)malloc(sizeof(char)*20);
    writes = (char*)malloc(sizeof(char)*100);
    strcpy(writee, mspace);
    strcpy(writes, mspace);
    if(tmp->type!=PRINTF)
        error(tmp->linen,6);
    tmp = tmp->next;//(
    if(tmp->type!=LPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    if(tmp->type==STRING)
    {
        strcpy(writes,tmp->value);//字符串赋值到writes
        slen = strlen(tmp->value);//处理引号
        for(i=0; i<slen-2; i++)
            writes[i]=writes[i+1];
        writes[i] = 0;

        tmp = tmp->next;
        if(tmp->type==COMMA)
        {
            tmp = tmp->next;
            exprflag = 1;
            strcpy(tmpexpr,gentmpvar());
            expression(tmpexpr);
            strcpy(writee, tmpexpr);
        }
    }
    else
    {
        exprflag = 1;
        strcpy(tmpexpr,gentmpvar());
        expression(tmpexpr);
        strcpy(writee, tmpexpr);
    }
    if(tmp->type!=RPAR)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    tmp = tmp->next;

    if(exprflag == 1)
    {
        if(writetype == 0)//int
            genmid(mwrite, writes, mint ,writee);
        else
            genmid(mwrite, writes, mchar ,writee);
    }
    else
    {
        if(writetype == 0)//int
            genmid(mwrite, writes, mspace, writee);
        else
            genmid(mwrite, writes, mspace, writee);
    }

    fprintf(fgramout,"This is a write sentence!\n");
    free(writee);free(writes);
    writing = 0;
}

//＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]
void returnstatement()
{
    if(tmp->type!=RETURN)
    {
        error(tmp->linen,6);
        skip2sc();
        return;
    }
    tmp = tmp->next;//(
    if(tmp->type!=LPAR)//return;的情况
    {
        if(ismain==1)
            genmid(mmret,mspace,mspace,mspace);
        if(funcflag !=3&&ismain==0)//不是void函数直接返回，则报错
        {
            error(tmp->linen, 23);
            skip2sc();
            return;
        }
        genmid(mret, mspace, mspace, mspace);
        voidret = 1;
        fprintf(fgramout,"This is a return sentence!\n");
        return;
    }
    if(funcflag == 3)//返回了表达式但是是void函数，则报错
    {
        error(tmp->linen, 23);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    strcpy(tmpexpr,gentmpvar());
    expression(tmpexpr);
    if(funcflag == 0 &&expretype==1)
    {
        error(tmp->linen,23);
        skip2sc();
        return;
    }
    if(funcflag == 1 &&expretype==0)
    {
        error(tmp->linen,23);
        skip2sc();
        return;
    }
    genmid(mret, mspace, mspace, tmpexpr);
    if(tmp->type!=RPAR)
    {
        error(tmp->linen, 14);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    fprintf(fgramout,"This is a return sentence!\n");
}

//＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void mainstatement()
{
    ADDR = 0;
    ismain = 1;
    insert_sym("main",2,3,0,0,0);//函数添加符号表
    if(tmp->type!=VOID)
    {
        error(tmp->linen,18);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    if(tmp->type!=MAIN)
    {
        error(tmp->linen,18);
        skip2sc();
        return;
    }
    genmid(mfunc, mvoid, mspace, mmain);
    tmp = tmp->next;
    if(tmp->type!=LPAR)//(
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    tmp = tmp->next;
    if(tmp->type!=RPAR)//)
    {
        error(tmp->linen,14);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    if(tmp->type!=LBRACE)//{
    {
        printf("error: for word %s, ",tmp->value);
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    funcflag = 3;
    compoundstatement();
    if(tmp->type!=RBRACE)//}
    {
        error(tmp->linen,15);
        skip2sc();
        return;
    }
    else
        tmp = tmp->next;
    fprintf(fgramout,"This is main function sentence!\n");
}

/*===============================================表达式、项、因子======================================================================*/
//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}  返回表达式之后的下一个单词
void expression(char *dst)//避免出现结束一次flag就改变的情况
{
    expreinside();
    isarrayoffset = 0;
}

void expreinside()
{
    char mid1[20], mid2[20], mid3[20];//mid1=mid2,op,mid3
    int flag=1;
    if(isarrayoffset==0)
        expretype = 1;

    if(tmp->type==ADD || tmp->type==SUB)
    {
        if(tmp->type==SUB)
            flag = -1;
        tmp = tmp->next;
        if(isarrayoffset==0)
            expretype = 0;
        else
            arraytype[arraytypenum]=0;
    }

    term();
    if(flag == -1)//如果是负号的话需要修饰前面的项
    {
        strcpy(mid3, tmpexpr);
        strcpy(mid1, gentmpvar());
        genmid(msub, mzero, mid3, mid1);//mid1=0-mid3
        strcpy(mid2, mid1);
    }
    else
        strcpy(mid2, tmpexpr);
    if(tmp->type!=ADD && tmp->type!=SUB)
    {
        strcpy(mid1,mid2);
    }
    flag = 1;
    while(tmp->type==ADD || tmp->type==SUB)//运算符
    {
        if(isarrayoffset==0&&writing == 1&&iscalling==0)
            writetype = 0;
        if(isarrayoffset==0)
            expretype = 0;
        else
            arraytype[arraytypenum]=0;
        if(tmp->type==SUB)
            flag = -1;
        else
            flag = 1;
        tmp = tmp->next;
        term();
        strcpy(mid3, tmpexpr);

        strcpy(mid1,gentmpvar());
        if(flag>0)
            genmid(madd,mid2,mid3,mid1);
        else
            genmid(msub,mid2,mid3,mid1);
        strcpy(mid2, mid1);
    }
    strcpy(tmpexpr, mid1);
}
//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}   返回这项之后的下一个单词
void term()
{
    char mid1[20], mid2[20], mid3[20];
    int flag;
    factor();
    strcpy(mid2, tmpexpr);
    strcpy(mid1, tmpexpr);
    while(tmp->type==MUL || tmp->type==DIV)
    {
        if(isarrayoffset==0&&writing == 1&&iscalling==0)
            writetype = 0;
        if(isarrayoffset==0)
            expretype = 0;
        else
            arraytype[arraytypenum]=0;
        if(tmp->type==MUL)
            flag = 1;
        else
            flag = -1;
        tmp = tmp->next;
        factor();
        strcpy(mid3, tmpexpr);
        if(tmp->type==MUL || tmp->type==DIV)
        {
            strcpy(mid1,gentmpvar());
            if(flag == 1)
                genmid(mmul,mid2,mid3,mid1);
            else
                genmid(mdiv,mid2,mid3,mid1);
            strcpy(mid2, mid1);
        }
        else
        {
            strcpy(tmpexpr,gentmpvar());
            if(flag ==1)
                genmid(mmul,mid2,mid3,tmpexpr);
            else
                genmid(mdiv,mid2,mid3,tmpexpr);
        }
    }
}
//＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞
//｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’    返回因子结束后的单词
void factor()//(0，字符，1整数)
{
    char mid2[20], mid3[20], arraylen[20];
    int charvalue, i;//标识符在符号表的序号
    int para,arrayoffset = 0;

    if(tmp->type==IDENTITY && tmp->next->type!=LPAR)
    {
        strcpy(tmpexpr, tmp->value);
        //判断标识符是否重复
        if(search_sym(tmpexpr, 0)<0)
        {
            printf("error: for word %s, ",tmp->value);
            error(tmp->linen, 22);
            skip2sc();
            return;
        }
        i = search_sym(tmpexpr, 0);
        if((sym[i].type==0||(sym[i].type==2 && sym[i].value==0))&&isarrayoffset==0)
            expretype = 0;
        else if((sym[i].type==0||(sym[i].type==2 && sym[i].value==0))&&isarrayoffset==1)
            arraytype[arraytypenum]=0;
        para = sym[i].para;//数组的长度
        if((sym[i].type==0||sym[i].type==2 && sym[i].value==0)&&isarrayoffset==0&&writing == 1&&iscalling==0)
            writetype = 0;//输出类型
        tmp = tmp->next;//[
        if(tmp->type!=LSQB)//＜标识符＞
        {
            return;
        }

        else{//＜标识符＞‘[’＜表达式＞‘]’
            isarrayoffset = 1;
            arraytypenum ++;
            arraytype[arraytypenum]=1;
            tmp = tmp->next;//表达式
            strcpy(mid2, tmpexpr);//标识符

            if((tmp->type==14||tmp->type==15)&&tmp->next->type==23&&integer()>0)//判断静态数组
            {
                arrayoffset = atoi(tmp->value);
                if(para<=arrayoffset)
                {
                    error(tmp->linen,24);//越上界
                    strcpy(tmpexpr,"0");
                }
                if(arrayoffset<0)
                {
                    error(tmp->linen,24);//越下界
                    strcpy(tmpexpr,"0");
                }
            }

            strcpy(tmpexpr,gentmpvar());
            expreinside();//tmpexpr是数组元素位置 数组长度怎么检查？？
            if(arraytype[arraytypenum]==1)
            {
                printf("error: for word %s, ",tmp->value);
                error(tmp->linen, 11);
                skip2sc();
                return;
            }
            strcpy(arraylen, tmpexpr);
            isarrayoffset = 0;
            arraytypenum--;

            strcpy(tmpexpr,gentmpvar());
            genmid(mass, mid2, arraylen, tmpexpr);
            if(tmp->type!=RSQB)
            {
                error(tmp->linen,12);
                skip2sc();
                return;
            }
            else
                tmp = tmp->next;
        }
        return;
    }
    else if(integer()>0)//＜整数＞
    {
        if(isarrayoffset==0&&writing == 1&&iscalling==0)
            writetype = 0;
        if(isarrayoffset==0)
            expretype = 0;
        else
            arraytype[arraytypenum]=0;
        itoa(number,tmpexpr,10);
        tmp =tmp->next;
        return;
    }
    else if(tmp->type==CHARACTER)//＜字符＞
    {
        charvalue = (int)(tmp->value[1]);
        itoa(charvalue,tmpexpr,10);
        tmp =tmp->next;
        return;
    }
    else if(tmp->type==LPAR)//‘(’＜表达式＞‘)’
    {
        tmp = tmp->next;
        expreinside();
        if(tmp->type!=RPAR)
        {
            error(tmp->linen,14);
            skip2sc();
            return;
        }
        tmp = tmp->next;
        return;
    }
    else if(tmp->type==IDENTITY && tmp->next->type==LPAR)//＜有返回值函数调用语句＞
    {
        iscalling = 1;
        i = search_sym(tmpexpr, 0);
        if(sym[i].type==0&&isarrayoffset==0)
            expretype = 0;
        else if(sym[i].type==0&&isarrayoffset==1)
            arraytype[arraytypenum]=0;
        callretvstatement();
        iscalling = 0;
        return;
    }
    else
    {
        error(tmp->linen,25);
        skip2sc();
        return;
    }
}
