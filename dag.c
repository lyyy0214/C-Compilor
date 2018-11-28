#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
FILE *fdag;
typedef struct midco
{
    char op[10];	//操作符
	char scr1[128];	//第一个操作数
	char scr2[64];	//第二个操作数
    char dst[64];
}midco;
extern midco midcode[1000];
extern int midconum;

typedef struct opmidco
{
    char op[10];	//操作符
	char scr1[128];	//第一个操作数
	char scr2[64];	//第二个操作数
    char dst[64];
}opmidco;
opmidco opmidcode[1000];

typedef struct BasicBlock{//基本块结构
	int startnumber; //起始原中间代码序号
	int endnumber;//结束原中间代码序号
}BasicBlock;
BasicBlock basicblock[1000];


typedef struct dagNode{
	int isExported;//判断是否打印过
	int tablenumber;//在节点表中的节点号
	int leftNumber;//左孩子dag号
	int rightNumber;//右孩子dag号
	char name[100];//节点名称
	int mid;//当为0的时候，表明为叶节点，当为1的时候，表明为中间节点。
}dagNode;
dagNode dagnode[1000];

typedef struct Ntable{
	int dagNumber;//该节点在dag图中的序号
	char nodeName[100];
	int nodeoff;//该节点的位移的序号
}Ntable;
Ntable nodetable[1000];

int opmidcodenum = 0;//生成的优化后的代码数量
int basicblocknum = 0;//基本块的数量
int dagnodenum = 0;//dag图中的节点数量
int nodetablenum = 0;//节点表中的节点数量
int deriveline[2048];//导出的节点列 还需逆序,内容是dag图中的序号
int derivelinenum = 0;

void make_block();//划分基本块
void produce_dag();//生成dag图
int find_node(char *name);//在节点表中找节点
int find_dag(int l, int r);//在dag图中找中间节点
void derive_dag();//导出dag图
int father_derived(int child);//查看父节点是否均导出
int choose_derive();//选择一个节点导出
int var_derive(int dagnum, int start);//导出的变量
void combine_ass();
void finish();//结束一次基本块的划分

void dag_opti()
{
    int i;
    fdag=fopen("15231105_opmidcode.txt","w");
    if(fdag==NULL)
    {
        printf("Can't open 15231105_opmidcode.txt!\n");
        return;
    }
    make_block();
}

void make_block()//划分基本块
{
    int i = 0;
    while(i<midconum)
    {
        if(strcmp(midcode[i].op,"+")!=0 && strcmp(midcode[i].op,"-")!=0 && strcmp(midcode[i].op,"*")!=0 &&
           strcmp(midcode[i].op,"/")!=0 && strcmp(midcode[i].op,"ass")!=0 && strcmp(midcode[i].op,"assa")!=0)
        {
            strcpy(opmidcode[opmidcodenum].scr1,midcode[i].scr1);
            strcpy(opmidcode[opmidcodenum].scr2,midcode[i].scr2);
            strcpy(opmidcode[opmidcodenum].dst,midcode[i].dst);
            strcpy(opmidcode[opmidcodenum].op,midcode[i].op);
            opmidcodenum++;
            i++;
        }
        else
        {
            basicblock[basicblocknum].startnumber = i;//基本块表达式的起始
            while(strcmp(midcode[i].op,"+")==0 || strcmp(midcode[i].op,"-")==0 || strcmp(midcode[i].op,"*")==0 ||
                  strcmp(midcode[i].op,"/")==0 || strcmp(midcode[i].op,"ass")==0 || strcmp(midcode[i].op,"assa")==0)
            {
                i++;
            }
            basicblock[basicblocknum].endnumber = i;//结束的序号是i-1
            produce_dag();//生成dag图
            derive_dag();
            combine_ass;
            finish();
            basicblocknum++;
        }
    }
}

void produce_dag()
{
    int i,l,r,d,t;//d在dag图中的序号 t在节点表中的序号
    char array[100];
    int rt,j;
    i = basicblock[basicblocknum].startnumber;
    while(i<basicblock[basicblocknum].endnumber)//赋值语句需要单独处理
    {
        //左节点
        l = find_node(midcode[i].scr1);
        if(l<0)//第一个操作数不在节点表里,节点表&dag图中加入新节点
        {
            //节点表建点
            strcpy(nodetable[nodetablenum].nodeName,midcode[i].scr1);
            nodetable[nodetablenum].dagNumber = dagnodenum;
            nodetable[nodetablenum].nodeoff = -1;
            //dag图建点
            strcpy(dagnode[dagnodenum].name,midcode[i].scr1);
            dagnode[dagnodenum].mid = 0;
            dagnode[dagnodenum].leftNumber = -1;
            dagnode[dagnodenum].rightNumber = -1;
            dagnode[dagnodenum].isExported = 0;
            dagnode[dagnodenum].tablenumber = nodetablenum;
            l = dagnodenum;

            nodetablenum++,dagnodenum++;
        }
        else//节点表中存在该节点
        {
            //如果是数组元素，则再次寻找更准确的节点
            if(strcmp(midcode[i].op,"ass")==0 && strcmp(midcode[i].scr2,"\0")!=0)
            {
                j = l;//j扫描一遍nodetable
                while(j<nodetablenum)
                {
                    if(strcmp(nodetable[j].nodeName,midcode[i].scr1)==0)//数组名相同
                    {
                        strcpy(array,nodetable[nodetable[j].nodeoff].nodeName);
                        if(strcmp(array,midcode[i].scr2)==0)
                            break;
                    }
                    j++;
                }
                if(j!=nodetablenum)
                    l = j;
                l = nodetable[l].dagNumber;
            }
            else
            {
                l = nodetable[l].dagNumber;
            }
        }

        //右节点
        if(strcmp(midcode[i].scr2,"\0")!=0)
        {
            r = find_node(midcode[i].scr2);
            rt = r;//rt保存右孩子在节点表中的节点
            if(r<0)//第2个操作数不在节点表里,节点表&dag图中加入新节点
            {
                //节点表建点
                strcpy(nodetable[nodetablenum].nodeName,midcode[i].scr2);
                nodetable[nodetablenum].dagNumber = dagnodenum;
                nodetable[nodetablenum].nodeoff = -1;
                //dag图建点
                strcpy(dagnode[dagnodenum].name,midcode[i].scr2);
                dagnode[dagnodenum].mid = 0;
                dagnode[dagnodenum].leftNumber = -1;
                dagnode[dagnodenum].rightNumber = -1;
                dagnode[dagnodenum].isExported = 0;
                dagnode[dagnodenum].tablenumber = nodetablenum;
                r = dagnodenum;
                rt = r;

                nodetablenum++,dagnodenum++;
            }
            else//节点表中存在该节点
            {
                r = nodetable[r].dagNumber;
            }
        }
        else
        {
            r = -1;
            rt = -1;
        }

        //dst
        t = find_node(midcode[i].dst);
        if(t<0)//dst节点需要建立
        {
            strcpy(nodetable[nodetablenum].nodeName,midcode[i].dst);
            t = nodetablenum;
            nodetable[nodetablenum].nodeoff = rt;
            nodetablenum++;
        }
        else if(strcmp(midcode[i].op,"assa")==0)//数组赋值遇到了相同的命名
        {
            strcpy(array,nodetable[nodetable[t].nodeoff].nodeName);
            if(strcmp(array,midcode[i].scr2)==0)
                ;
            else
            {
                strcpy(nodetable[nodetablenum].nodeName,midcode[i].dst);
                t = nodetablenum;
                nodetable[nodetablenum].nodeoff = rt;//scr2即数组位移在节点表中的序号
                nodetablenum++;
            }
        }

        d = find_dag(l,r);//先在dag图中看看有没有dst,同时查看符号是否相同
        if(d<0||strcmp(midcode[i].op,dagnode[d].name)!=0||(strcmp(midcode[i].op,"ass")!=0&&strcmp(midcode[i].scr2,"\0")==0))
        {
            //dag图中建立中间节点
            strcpy(dagnode[dagnodenum].name,midcode[i].op);
            dagnode[dagnodenum].mid = 1;
            dagnode[dagnodenum].leftNumber = l;
            dagnode[dagnodenum].rightNumber = r;
            dagnode[dagnodenum].isExported = 0;
            dagnode[dagnodenum].tablenumber = -1;
            d = dagnodenum;
            dagnodenum++;
        }
        else if(strcmp(midcode[i].op,"ass")==0)
        {
            if(isdigit(midcode[i].scr1[0])==0&&midcode[i].scr1[0]!='-'
               &&midcode[i].scr1[0]!='$')//变量增加节点
            {
                strcpy(dagnode[dagnodenum].name,midcode[i].op);
                dagnode[dagnodenum].mid = 1;
                dagnode[dagnodenum].leftNumber = l;
                dagnode[dagnodenum].rightNumber = r;
                dagnode[dagnodenum].isExported = 0;
                dagnode[dagnodenum].tablenumber = -1;
                d = dagnodenum;
                dagnodenum++;
            }
            else if( dagnode[d].mid == 0)//叶节点有一个以上的变量就需要导出
            {
                dagnode[d].mid = 2;
            }
        }
        nodetable[t].dagNumber = d;
        if(dagnode[d].tablenumber == -1)//新建立的dag图中的节点
            dagnode[d].tablenumber = t;
        else//之前建立过，看看能不能把临时变量替换成普通变量
        {
            /*if(nodetable[dagnode[d].tablenumber].nodeName[0]=='$' &&
               nodetable[dagnode[d].tablenumber].nodeName[1]=='m')*/
            if(dagnode[d].mid==1)
                dagnode[d].tablenumber = t;
        }

        i++;
    }
}

void derive_dag()
{
    int i = 0,j=0,first = 0,derivelinenum = 0, need_derivenum,reversed[2048],len;
    int target,derive_target,var_target;
    int l,r,d;

    while(j<dagnodenum)
    {
        if(dagnode[j].mid==0)
        {
            dagnode[j].isExported = 1;
            i++;
        }
        j++;
    }
    need_derivenum = dagnodenum-i;
    while(derivelinenum<need_derivenum)
    {
        target = choose_derive();
        deriveline[derivelinenum] = target;
        dagnode[target].isExported = 1;
        derivelinenum++;
    }

    //reverse
    len = derivelinenum;
    for(i = 0; i < len; i++)
    {
        reversed[i] = deriveline[len-i-1];
    }

    for(i = 0; i < len; i++)//依次导出i
    {
        derive_target = reversed[i];
        dagnode[derive_target].isExported = 1;
        l = dagnode[derive_target].leftNumber;//dag图中左孩子节点在dag图中的序号
        r = dagnode[derive_target].rightNumber;
        d = dagnode[derive_target].tablenumber;
        if(l>=0 && r>=0)
        {
            l = dagnode[l].tablenumber;
            r = dagnode[r].tablenumber;
            strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//左
            strcpy(opmidcode[opmidcodenum].scr2,nodetable[r].nodeName);//右
            strcpy(opmidcode[opmidcodenum].dst,nodetable[d].nodeName);
            strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
            opmidcodenum++;
            j = 0;
            while(var_derive(derive_target,j)>=0)//导出节点表中其他的非临时变量
            {
                var_target = var_derive(derive_target,j);
                strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//左
                strcpy(opmidcode[opmidcodenum].scr2,nodetable[r].nodeName);//右
                strcpy(opmidcode[opmidcodenum].dst,nodetable[var_target].nodeName);
                strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
                opmidcodenum++;
                j = var_target+1;
            }
        }
        else if(r<0&&l>=0)//a=b
        {
            l = dagnode[l].tablenumber;
            strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//左
            strcpy(opmidcode[opmidcodenum].scr2,"\0");//右
            strcpy(opmidcode[opmidcodenum].dst,nodetable[d].nodeName);
            strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
            opmidcodenum++;
        }
        else//节点中有其他变量
        {
            j = 0, first = 0;
            while(j<nodetablenum)
            {
                if(nodetable[j].dagNumber==derive_target)
                    break;
                j++;
            }
            first = j;
            for(j=j+1;j<nodetablenum;j++)
            {
                if(nodetable[j].dagNumber==derive_target)
                {
                    strcpy(opmidcode[opmidcodenum].scr1,nodetable[first].nodeName);//左
                    strcpy(opmidcode[opmidcodenum].scr2,"\0");//右
                    strcpy(opmidcode[opmidcodenum].dst,nodetable[j].nodeName);
                    strcpy(opmidcode[opmidcodenum].op,"ass");
                    opmidcodenum++;
                }
            }
        }

    }
}

void combine_ass()
{
    int i,j=0;
    for(i = 0;i<opmidcodenum-1;i++)
    {
        if(strcmp(opmidcode[i].op,"+")==0||strcmp(opmidcode[i].op,"-")==0
           ||strcmp(opmidcode[i].op,"*")==0||strcmp(opmidcode[i].op,"/")==0)
        {
            if(strcmp(opmidcode[i+1].op,"ass")==0&&strcmp(opmidcode[i+1].scr2,"\0")==0
               &&strcmp(opmidcode[i].dst,opmidcode[i+1].scr1)==0 && opmidcode[i].dst[0]=='$')
            {
                strcpy(midcode[j].op,opmidcode[i].op);
                strcpy(midcode[j].scr1,opmidcode[i].scr1);
                strcpy(midcode[j].scr2,opmidcode[i].scr2);
                strcpy(midcode[j++].dst,opmidcode[i+1].dst);
                i++;//一次跳过两条
            }
            else
            {
                strcpy(midcode[j].op,opmidcode[i].op);
                strcpy(midcode[j].scr1,opmidcode[i].scr1);
                strcpy(midcode[j].scr2,opmidcode[i].scr2);
                strcpy(midcode[j++].dst,opmidcode[i].dst);
            }
        }
        else
        {
            strcpy(midcode[j].op,opmidcode[i].op);
            strcpy(midcode[j].scr1,opmidcode[i].scr1);
            strcpy(midcode[j].scr2,opmidcode[i].scr2);
            strcpy(midcode[j++].dst,opmidcode[i].dst);
        }
    }
    strcpy(midcode[j].op,opmidcode[i].op);
    strcpy(midcode[j].scr1,opmidcode[i].scr1);
    strcpy(midcode[j].scr2,opmidcode[i].scr2);
    strcpy(midcode[j++].dst,opmidcode[i].dst);
    midconum = j;
    for(i=0;i<midconum;i++)
    {
        strcpy(opmidcode[i].op,midcode[i].op);
        strcpy(opmidcode[i].scr1,midcode[i].scr1);
        strcpy(opmidcode[i].scr2,midcode[i].scr2);
        strcpy(opmidcode[i].dst,midcode[i].dst);
    }
    opmidcodenum = midconum;
}

int choose_derive()
{
    int i=dagnodenum-1;
    while(i>=0)
    {
        if(dagnode[i].mid>0 && father_derived(i)==1 && dagnode[i].isExported==0)//不是叶子节点且父节点均导出
            return i;
        i--;
    }
    return -1;
}

int father_derived(int child)//看child的父节点是否都导出了
{
    int i = 0,l,r;

    while(i<dagnodenum)
    {
        l = dagnode[i].leftNumber;
        r = dagnode[i].rightNumber;
        if(l==child)
        {
            if(dagnode[i].isExported==0)
                return -1;
        }
        else if(r==child)
        {
            if(dagnode[i].isExported==0)
                return -1;
        }
        i++;
    }
    return 1;
}

int var_derive(int dagnum, int start)
{
    int i = 0;
    i = start;
    while(i<nodetablenum)
    {
        if(nodetable[i].dagNumber == dagnum && dagnode[dagnum].tablenumber!=i)//先判断对应的dag图序号是dagnum
        {
            if(nodetable[i].nodeName[0]!='$' && nodetable[i].nodeName[1]!='m')
                return i;
        }
        i++;
    }
    return -1;
}

int find_node(char *name)
{
    int i = 0;
    while(i<nodetablenum)
    {
        if(strcmp(name,nodetable[i].nodeName)==0)
            return i;
        i++;
    }
    return -1;
}

int find_dag(int l, int r)
{
    int i = 0;
    while(i<dagnodenum)
    {
        if(dagnode[i].leftNumber==l&&dagnode[i].rightNumber==r)
            return i;
        else if(i==l&&r==-1)//普通赋值语句左节点为该节点，右节点为-1
        {
            return i;
        }
        i++;
    }
    return -1;
}

void finish()
{
    int i;
    for(i = 0;i<dagnodenum;i++)
    {
        strcpy(dagnode[i].name,"\0");
        dagnode[i].isExported = 0;
        dagnode[i].leftNumber = -1;
        dagnode[i].rightNumber = -1;
        dagnode[i].tablenumber = -1;
        dagnode[i].mid = 0;
    }
    dagnodenum = 0;
    for(i = 0;i<nodetablenum;i++)
    {
        strcpy(nodetable[i].nodeName,"\0");
        nodetable[i].dagNumber = -1;
        nodetable[i].nodeoff = -1;
    }
    nodetablenum = 0;
    derivelinenum = 0;
}

