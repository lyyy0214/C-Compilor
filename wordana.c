#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "type.h"

#define resvnum 13

typedef struct Wnode
{
    int num;
    int type;
    int linen;
    char value[vmax];
    struct Wnode *next;
}Wnode;

Wnode *head,*wordlist,*lastword;
char input[100000],skip[100000];
char reserver[resvnum][10]={"const","int","char","void","main","if","do","while","switch","case","scanf","printf","return"};
//input为编译的程序,reserver为保留字
int len=0,wordnum=1;//编号
int linen = 1;

void add(int start,int addlen,int ty);
void output();
extern void error(int pos, int errortype);

void wordana()
{
    int i=0,d=0,k,start=0,addlen,state=0;
    char id[vmax],check[vmax];
    char file_path[100];
    printf("please insert the path of your file!\n");
    scanf("%s",&file_path);
    fin=fopen(file_path,"r");
    if(fin==NULL)
    {
        printf("Can't open 15231105_test.txt!\n");
        return 0;
    }
    fout=fopen("15231105_word.txt","w");
    if(fout==NULL)
    {
        printf("Can't open 15231105_output.txt!\n");
        return 0;
    }

    while(!feof(fin))
        input[i++]=fgetc(fin);
    len=i;//len为输入文件的长度

    for(i = 0;i<len;i++)
        skip[i] = 0;
    i=0;

    while(input[i]!='\0')
    {
        //跳过全部排版时的空格、换行和制表符
        while(input[i]==' '||input[i]=='\t'||input[i]<0)
        {
            i++;
        }
        if(input[i]=='\n')
        {
            linen++;
            i++;
        }
        if((input[i]<='z'&&input[i]>='a')||(input[i]<='Z'&&input[i]>='A')||input[i]=='_')//标识符
        {
            d=0;
            id[0]=0;//清空字符串
            start=i;
            addlen=1;
            id[d++]=input[i++];
            while(input[i]=='_'||(input[i]<='z'&&input[i]>='a')||(input[i]<='Z'&&input[i]>='A')||(input[i]<='9'&&input[i]>='0'))
            {
                id[d++]=input[i];
                i++;
                addlen++;
            }
            for(d=0;d<addlen;d++)
                check[d]=tolower(id[d]);
            check[d]='\0';
            id[d]='\0';
            for(k=0;k<resvnum;k++)//判断标识符是否为保留字
            {
                if(strcmp(check,reserver[k])==0)//找到保留字
                {
                    add(start,strlen(reserver[k]),k);//挂链,k为保留字号码
                    break;
                }
            }
            if(k==resvnum)
            {
                add(start,strlen(id),13);
            }
            //break;
        }
        else if(input[i]>='1'&&input[i]<='9')//数字
        {
            start=i;
            i++;
            addlen=1;
            while(input[i]>='0'&&input[i]<='9')
            {
                addlen++;
                i++;
            }
            add(start,addlen,14);
            //break;
        }
        else if(input[i]=='0')  // 0
        {
            start = i;
            if(input[i+1]<='9' && input[i+1]>='0')//0后面不可以接数字
            {
                error(linen,5);
                do
                    i++;
                while(input[i]<='9' && input[i]>='0');
                add(start,1,15);
            }
            else{
                add(start,1,15);
                i++;//break;
            }
        }
        else if(input[i]==34)//字符串
        {
            start=i;
            i++;
            addlen=1;
            while(input[i]!=34)
            {
                if(input[i]==32||input[i]==33||(input[i]>=35&&input[i]<=126))
                {
                    i++;
                    addlen++;
                }
                else if(input[i]=='\n'||input[i]==0||input[i]==';'||input[i]==','||input[i]==')')
                {
                    error(linen,0);
                    state = 1;
                    skip[i] = 1;
                    i++;
                    break;
                }
                else if(input[i]==34)
                    break;
                else
                {
                    error(linen,0);
                    skip[i] = 1;
                    addlen++;
                    i++;
                    state=1;
                }
            }
            if(input[i]==34&&state==0)//遇到右边的引号
            {
                addlen++;
                add(start,addlen,16);
                i++;
            }
            else
            {
                error(linen,0);
                addlen++;
                add(start,addlen,16);
                i++;
            }
            state=0;
        }
        else if(input[i]==39)//字符
        {
            if(input[i+2]!=39)//缺少另一个‘
            {
                if(input[i+1]=='+'||input[i+1]=='-'||input[i+1]=='*'||input[i+1]=='/'||isalnum(input[i+1])!=0||input[i+1]=='_')
                {
                    add(i,2,17);//只缺少右引号则保留这个字符即可
                    error(linen,1);
                    i = i+2;
                }
                else
                {
                    error(linen,1);
                    i++;
                }
                while(input[i]!='\n'&&input[i]!=0&&input[i]!=' '&&input[i]!=';'&&input[i]!=',')
                    i++;
            }
            //非法字符
            else if(input[i+1]!='+'&&input[i+1]!='-'&&input[i+1]!='*'&&input[i+1]!='/'&&isalnum(input[i+1])==0&&input[i+1]!='_')
            {
                error(linen,2);
                i = i+2;
            }
            else{
                add(i,3,17);
                i=i+3;
            }
        }
        else if(input[i]=='(')
        {
            add(i,1,18);
            i++;
        }
        else if(input[i]==')')
        {
            add(i,1,19);
            i++;
        }
        else if(input[i]=='{')
        {
            add(i,1,20);
            i++;
        }
        else if(input[i]=='}')
        {
            add(i,1,21);
            i++;
        }
        else if(input[i]=='[')
        {
            add(i,1,22);
            i++;
        }
        else if(input[i]==']')
        {
            add(i,1,23);
            i++;
        }
        else if(input[i]=='+')
        {
            add(i,1,24);
            i++;
        }
        else if(input[i]=='-')
        {
            add(i,1,25);
            i++;
        }
        else if(input[i]=='*')
        {
            add(i,1,26);
            i++;
        }
        else if(input[i]=='/')
        {
            add(i,1,27);
            i++;
        }
        else if(input[i]=='<')
        {
            if(input[i+1]=='=')
            {
                add(i,2,29);
                i=i+2;
            }
            else
            {
                add(i,1,30);
                i++;
            }
        }
        else if(input[i]=='>')
        {
            if(input[i+1]=='=')
            {
                add(i,2,31);
                i=i+2;
            }
            else
            {
                add(i,1,32);
                i++;
            }
        }
        else if(input[i]=='=')
        {
            if(input[i+1]=='=')
            {
                add(i,2,33);
                i=i+2;
            }
            else
            {
                add(i,1,28);
                i++;
            }
        }
        else if(input[i]=='!')
        {
            if(input[i+1]=='=')
            {
                add(i,2,34);
                i=i+2;
            }
            else
            {
                error(linen,3);
                add(i,2,34);
                i++;
            }
        }
        else if(input[i]==';')
        {
            add(i,1,35);
            i++;
        }
        else if(input[i]==':')
        {
            add(i,1,36);
            i++;
        }
        else if(input[i]==',')
        {
            add(i,1,37);
            i++;
        }
        else if(input[i]==' '||input[i]=='\t'||input[i]<0)
            i++;
        else if(input[i]=='\n')
        {
            i++;
            linen++;
        }
        else if(input[i]=='\0')
            break;
        else{
            error(linen,4);
            i++;
        }
    }
    lastword->next=NULL;
    wordnum--;
    output();
}
/*---------------增加一个新的输出单词的节点--------------------*/
void add(int start,int addlen,int ty)
{
    int d,newnoden = 0;
    char newnode[1000];
    Wnode *newword;
    newword=(Wnode *)malloc(sizeof(Wnode));
    newword->type=ty;
    newword->linen = linen;
    newword->num=wordnum;
    if(ty<=13)
    {
        for(d=start;d<start+addlen;d++)
        {
            if(skip[d]==1)
            {
                continue;
            }
            if(input[d]>='A' && input[d]<='Z')
                input[d]=tolower(input[d]);
            newnode[newnoden++] = input[d];
        }
        newnode[newnoden] = 0;
        strcpy(newword->value,newnode);
    }
    else if(ty==34)//!=错误的情况根据类别直接填！=
        strcpy(newword->value,"!=");
    else
    {
        for(d=start;d<start+addlen;d++)
        {
            if(skip[d]==1)
            {
                continue;
            }
            newnode[newnoden++] = input[d];
        }
        newnode[newnoden] = 0;
        strcpy(newword->value,newnode);
    }
    newword->value[addlen]='\0';
    if(wordnum!=1)
        lastword->next=newword;
    else
        head=newword;
    lastword=newword;
    wordnum++;
}

/*---------------输出词法分析--------------------*/
void output()
{
    int i;
    Wnode *tmp;
    tmp=head;
    for(i=0;i<wordnum;i++){
        fprintf(fout,"%d ",i+1);
        switch (tmp->type)
        {
            case 0:{fprintf(fout,"CONST");break;}
            case 1:{fprintf(fout,"INT");break;}
            case 2:{fprintf(fout,"CHAR");break;}
            case 3:{fprintf(fout,"VOID");break;}
            case 4:{fprintf(fout,"MAIN");break;}
            case 5:{fprintf(fout,"IF");break;}
            case 6:{fprintf(fout,"DO");break;}
            case 7:{fprintf(fout,"WHILE");break;}
            case 8:{fprintf(fout,"SWITCH");break;}
            case 9:{fprintf(fout,"CASE");break;}
            case 10:{fprintf(fout,"SCANF");break;}
            case 11:{fprintf(fout,"PRINTF");break;}
            case 12:{fprintf(fout,"RETURN");break;}

            case 13:{fprintf(fout,"IDENTITY");break;}
            case 14:{fprintf(fout,"NUMBER");break;}
            case 15:{fprintf(fout,"ZERO");break;}
            case 16:{fprintf(fout,"STRING");break;}
            case 17:{fprintf(fout,"CHARACTER");break;}

            case 18:{fprintf(fout,"LPAR");break;}
            case 19:{fprintf(fout,"RPAR");break;}
            case 20:{fprintf(fout,"LBRACE");break;}
            case 21:{fprintf(fout,"RBRACE");break;}
            case 22:{fprintf(fout,"LSQB");break;}
            case 23:{fprintf(fout,"RQB");break;}

            case 24:{fprintf(fout,"ADD");break;}
            case 25:{fprintf(fout,"SUB");break;}
            case 26:{fprintf(fout,"MUL");break;}
            case 27:{fprintf(fout,"DIV");break;}
            case 28:{fprintf(fout,"ASS");break;}

            case 29:{fprintf(fout,"LEQ");break;}
            case 30:{fprintf(fout,"LE");break;}
            case 31:{fprintf(fout,"GTQ");break;}
            case 32:{fprintf(fout,"GT");break;}
            case 33:{fprintf(fout,"EQU");break;}
            case 34:{fprintf(fout,"NEQ");break;}

            case 35:{fprintf(fout,"SEMICOL");break;}
            case 36:{fprintf(fout,"COLON");break;}
            case 37:{fprintf(fout,"COMMA");break;}
            default:{fprintf(fout,"UNDEFINE");break;}
        }
        fprintf(fout," %s\n",tmp->value);
        tmp=tmp->next;
    }
}

