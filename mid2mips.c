#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/*=============================中间代码操作符==================================*/
extern int op_onoff;
FILE *fmips;
int midnum = 0;//转化完成的中间码数量
int Global_String_num = 0;//字符串的数量
int func_head_para = 0;//函数中声明了的参数数量
int tmp_func_para = 0;//当前函数声明了多少参量
int func_push_para = 0;//调用函数时已经传参i=func(a,b)的数量
extern int midconum;
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

typedef struct midco
{
    char op[10];	//操作符
	char scr1[128];	//第一个操作数
	char scr2[64];	//第二个操作数
    char dst[64];
}midco;
extern midco midcode[1000];

typedef struct Global_const
{
    char name[32];
    char value[128];
}Global_const;

typedef struct Local_var
{
    char name[32];
    char funcname[32];
    int addr;
}Local_var;

Local_var Local_Var[1024];
Global_const Global_Const[1024];

int Global_Const_num = 0;
int Local_Var_num = 0;
int offset = 0;
char FUNCNAME[32];
/////////////////////////////////////////////////////////////////////////////////
int find_addr(char *NAME)
{
    int rt,i;
    for(i = 0; i<Local_Var_num;i++)
    {
        if(strcmp(NAME, Local_Var[i].name)==0 && strcmp(Local_Var[i].funcname,FUNCNAME)==0)
            return Local_Var[i].addr;
    }
    return -1;
}

void insert_var(char *NAME)
{
    strcpy(Local_Var[Local_Var_num].name, NAME);
    Local_Var[Local_Var_num].addr = offset;
    strcpy(Local_Var[Local_Var_num].funcname,FUNCNAME);
    Local_Var_num++;
    offset = offset+4;
}

//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////函数部分///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void init_mips();
void clean_mips();
void mips_func();
void mips_call();
void mips_para();
void mips_push();
void mips_assret();
void mips_ass();
void mips_assa();
void mips_label();
void mips_var();
void mips_const();
void mips_judge();
void mips_branch();
void mips_jmp();
void mips_scf();
void mips_ptf();
void mips_ret();
void mips_mret();
void mips_asmd();//+-*/
/*======================================主函数=================================================*/
void mid2mips(int op)
{
    if(op_onoff==0 || op==0)
    {
        fmips=fopen("15231105_mips.txt","w");
        if(fmips==NULL)
        {
            printf("Can't open 15231105_mips.txt!\n");
            return;
        }
    }
    else if(op_onoff==1 && op==1)
    {
        fmips=fopen("15231105_opdagmips.txt","w");
        if(fmips==NULL)
        {
            printf("Can't open 15231105_opdagmips.txt!\n");
            return;
        }
    }
    strcpy(FUNCNAME,"\0");
    init_mips();
    while(midnum<midconum)
    {
        if(strcmp(midcode[midnum].op, "func") == 0)
            mips_func();
        else if(strcmp(midcode[midnum].op, "call") == 0)
            mips_call();
        else if(strcmp(midcode[midnum].op, "para") == 0)
            mips_para();
        else if(strcmp(midcode[midnum].op, "push") == 0)
            mips_push();
        else if(strcmp(midcode[midnum].op, "assret") == 0)
            mips_assret();
        else if(strcmp(midcode[midnum].op, "ass") == 0)
            mips_ass();
        else if(strcmp(midcode[midnum].op, "assa") == 0)
            mips_assa();
        else if(strcmp(midcode[midnum].op, "label") == 0 )
            mips_label();
        else if(strcmp(midcode[midnum].op, "var") == 0 )
            mips_var();
        else if(strcmp(midcode[midnum].op, "const") == 0 )
            mips_const();
        else if(strcmp(midcode[midnum].op, "scf") == 0 )
            mips_scf();
        else if(strcmp(midcode[midnum].op, "==") == 0 || strcmp(midcode[midnum].op, "!=")==0
                || strcmp(midcode[midnum].op, "<") == 0 || strcmp(midcode[midnum].op, "<=")==0
                || strcmp(midcode[midnum].op, ">") == 0 || strcmp(midcode[midnum].op, ">=")==0)
            mips_judge();
        else if(strcmp(midcode[midnum].op, "bz") == 0 ||strcmp(midcode[midnum].op, "bnz") == 0)
            mips_branch();
        else if(strcmp(midcode[midnum].op, "jmp") == 0 )
            mips_jmp();
        else if(strcmp(midcode[midnum].op, "ptf") == 0 )
            mips_ptf();
        else if(strcmp(midcode[midnum].op, "ret") == 0 )
            mips_ret();
        else if(strcmp(midcode[midnum].op, "mainret") == 0 )
            mips_mret();
        else if(strcmp(midcode[midnum].op, "+") == 0 || strcmp(midcode[midnum].op, "-")==0
                || strcmp(midcode[midnum].op, "*") == 0 || strcmp(midcode[midnum].op, "/")==0)
            mips_asmd();
        midnum++;
    }

    fprintf(fmips,"\t\tli\t$v0\t10\n");//结束
    fprintf(fmips,"\t\tsyscall\n");
    clean_mips();
}

void mips_func()
{
    int funcid;
    funcid = search_sym(midcode[midnum].dst,1);
    func_head_para = sym[funcid].para;
    tmp_func_para = func_head_para;
    offset = 8;//修改8 func ret
    fprintf(fmips,"%s:\n",midcode[midnum].dst);//生成label
    strcpy(FUNCNAME,midcode[midnum].dst);//当前函数改变
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t8\n");//处理栈指针
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t%d\n",func_push_para*4);
}

void mips_call()//调用函数（和ret呼应）
{
    int funcid;
    funcid = search_sym(midcode[midnum].dst,1);
    func_head_para = sym[funcid].para;
    offset = offset - func_head_para*4;
    func_push_para = 0;
    fprintf(fmips,"\t\tsw\t$fp\t($sp)\n");//保存fp
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    fprintf(fmips,"\t\tsw\t$ra\t($sp)\n");//保存ra
    fprintf(fmips,"\t\tmove\t$fp\t$sp\n");//fp = sp
    fprintf(fmips,"\t\tjal\t%s\n",midcode[midnum].dst);//跳到函数
    fprintf(fmips,"\t\tnop\n");
}

void mips_para()
{
    int addr;
    fprintf(fmips,"\t\tlw\t$t0\t%d($fp)\n",4+4*func_head_para);
    insert_var(midcode[midnum].dst);
    addr = find_addr(midcode[midnum].dst);
    fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    func_head_para--;
}

void mips_push()
{
    int addr;
    addr = find_addr(midcode[midnum].dst);
    if(isdigit(midcode[midnum].dst[0])==0 && midcode[midnum].dst[0]!='-')//不是数字
    {
        if(addr != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t8\t-%d($fp)\n",addr);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t8\t%s\n",midcode[midnum].dst);//从全局变量中找到存进t8
            fprintf(fmips,"\t\tlw\t$t8\t($t8)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t8\t%s\n",midcode[midnum].dst);
    fprintf(fmips,"\t\tsw\t$t8\t($sp)\n");//下个函数的起始区域的相应位置
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    offset+=4;
    func_push_para++;
}

void mips_assret()
{
    int addr;
    insert_var(midcode[midnum].dst);//i=ret
    addr = find_addr(midcode[midnum].dst);
    fprintf(fmips,"\t\tsw\t$v1\t-%d($fp)\n",addr);//将返回值v1存
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
}

void mips_ass()//a=b[i]
{
    int addr1,addr2,addr3;//add1=b-t0 addr2=i-t1 addr3=a-t2
    int isglobal=0;

    if(strcmp(midcode[midnum].scr2,"\0")==0)//不是数组赋值 a=b
    {
        if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//不是数字
        {
            addr1 = find_addr(midcode[midnum].scr1);
            if(addr1 != -1)//不是全局变量
                fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)\n",addr1);
            else//全局变量
            {
                fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);//从全局变量中找到存进t0
                fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
            }
        }
        else
            fprintf(fmips,"\t\tli\t$t0\t%s\n",midcode[midnum].scr1);

        if(midcode[midnum].dst[0]=='$')//
        {
            insert_var(midcode[midnum].dst);
            addr3 = find_addr(midcode[midnum].dst);
            fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
            fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
        }
        else
        {
            addr3 = find_addr(midcode[midnum].dst);
            if(addr3 != -1)//不是全局变量
                fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
            else//全局变量
            {
                fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].dst);//从全局变量中找到存进t2
                fprintf(fmips,"\t\tsw\t$t0\t($t2)\n");
            }
        }
    }

    else
    {
        //b的位置
        addr1 = find_addr(midcode[midnum].scr1);
        if(addr1 != -1)//不是全局变量
        {
            isglobal = 0;
            fprintf(fmips,"\t\tli\t$t0\t-%d\n",addr1);
            fprintf(fmips,"\t\tadd\t$t0\t$t0\t$fp\n");
        }
        else//全局变量
        {
            isglobal = 1;
            fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);//从全局变量中找到存进t0
        }
        //数组偏移量t1
        addr2 = find_addr(midcode[midnum].scr2);
        if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//不是数字
        {
            if(addr2 != -1)//不是全局变量
                fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
            else//全局变量
            {
                fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//从全局变量中找到存进t1
                fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
            }
        }
        else
            fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);
        if(isglobal == 1)//全局变量
            fprintf(fmips,"\t\tmul\t$t1\t$t1\t4\n");
        else
            fprintf(fmips,"\t\tmul\t$t1\t$t1\t-4\n");
        fprintf(fmips,"\t\tadd\t$t0\t$t0\t$t1\n");
        fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        insert_var(midcode[midnum].dst);
        addr2 = find_addr(midcode[midnum].dst);
        fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr2);
    }
}

void mips_assa()//a[i] = b
{
    int addr1,addr2,addr3;//addr1-b t0 addr2-i t1 addr3-a t2
    addr3 = find_addr(midcode[midnum].dst);

    if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//不是数字
    {
        addr1 = find_addr(midcode[midnum].scr1);
        if(addr1 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)\n",addr1);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);//从全局变量中找到存进t0
            fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t0\t%s\n",midcode[midnum].scr1);

    if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//不是数字
    {
        addr2 = find_addr(midcode[midnum].scr2);
        if(addr2 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//从全局变量中找到存进t1
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);

    if(addr3 != -1)//局部变量
    {
        fprintf(fmips,"\t\tmul\t$t1\t$t1\t-4\n");
        fprintf(fmips,"\t\tli\t$t2\t-%d\n",addr3);
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$t2\n");
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$fp\n");
    }
    else
    {
        fprintf(fmips,"\t\tmul\t$t1\t$t1\t4\n");
        fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].dst);
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$t2\n");
    }
    fprintf(fmips,"\t\tsw\t$t0\t($t1)\n");
}

void mips_var()
{
    int arraylen;
    strcpy(Local_Var[Local_Var_num].name,midcode[midnum].dst);
    Local_Var[Local_Var_num].addr = offset;
    strcpy(Local_Var[Local_Var_num].funcname,FUNCNAME);
    Local_Var_num++;
    if(strcmp(midcode[midnum].scr2,"\0")==0)//不是数组
    {
        offset = offset + 4;
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    }
    else//数组
    {
        arraylen = atoi(midcode[midnum].scr2);
        offset = offset + 4*arraylen;
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t%d\n", 4*arraylen);//sp-4*arraylen
    }
}

void mips_const()
{
    int arraylen;
    strcpy(Local_Var[Local_Var_num].name,midcode[midnum].dst);
    strcpy(Local_Var[Local_Var_num].funcname,FUNCNAME);
    Local_Var[Local_Var_num].addr = offset;
    Local_Var_num++;
    //存值
    fprintf(fmips,"\t\tli\t$t0\t%s\n",midcode[midnum].scr2);
    fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",offset);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    offset += 4;
}

void mips_label()
{
    fprintf(fmips,"%s:\n",midcode[midnum].dst);//生成label
}

void mips_judge()
{
    int op;//0:== 1:!= 2:> 3:>= 4:< 5:<=
    int addr1,addr2;
    if(strcmp(midcode[midnum].op, "==") == 0 )//先设置符号op
        op = 0;
    else if(strcmp(midcode[midnum].op, "!=") == 0 )
        op = 1;
    else if(strcmp(midcode[midnum].op, ">") == 0 )
        op = 2;
    else if(strcmp(midcode[midnum].op, ">=") == 0 )
        op = 3;
    else if(strcmp(midcode[midnum].op, "<") == 0 )
        op = 4;
    else if(strcmp(midcode[midnum].op, "<=") == 0 )
        op = 5;
    /*-----------------------------------操作数1------------------------------*/
    if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//不是数字
    {
        addr1 = find_addr(midcode[midnum].scr1);
        if(addr1 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr1);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr1);//从全局变量中找到存进t0
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr1);
    /*-----------------------------------操作数2------------------------------*/
    if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//不是数字,scr2为常量
    {
        addr2 = find_addr(midcode[midnum].scr2);
        if(addr2 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t2\t-%d($fp)\n",addr2);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].scr2);//从全局变量中找到存进t1
            fprintf(fmips,"\t\tlw\t$t2\t($t2)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t2\t%s\n",midcode[midnum].scr2);

    if(op == 0)//==
        fprintf(fmips,"\t\tseq\t$t0\t$t1\t$t2\n");//满足给t0赋1
    if(op == 1)//!=
        fprintf(fmips,"\t\tsne\t$t0\t$t1\t$t2\n");
    if(op == 2)//>
        fprintf(fmips,"\t\tsgt\t$t0\t$t1\t$t2\n");
    if(op == 3)//>=
        fprintf(fmips,"\t\tsge\t$t0\t$t1\t$t2\n");
    if(op == 4)//<
        fprintf(fmips,"\t\tslt\t$t0\t$t1\t$t2\n");
    if(op == 5)//<=
        fprintf(fmips,"\t\tsle\t$t0\t$t1\t$t2\n");

}
void mips_branch()
{
    int branch;//1-等于1跳转 0-等于0跳转
    if(strcmp(midcode[midnum].op, "bz") == 0)//1跳转 bz不满足条件跳转
        fprintf(fmips,"\t\tbeq\t$t0\t0\t%s\n",midcode[midnum].dst);
    else
        fprintf(fmips,"\t\tbeq\t$t0\t1\t%s\n",midcode[midnum].dst);
}
void mips_jmp()
{
    fprintf(fmips,"\t\tj\t%s\n",midcode[midnum].dst);
    fprintf(fmips,"\t\tnop\n");
}

void mips_scf()
{
    int addr;
    if(strcmp(midcode[midnum].scr1, "int") == 0)
        fprintf(fmips,"\t\tli\t$v0\t5\n");//v0=5:read int
    else
        fprintf(fmips,"\t\tli\t$v0\t12\n");//v0=4:read character
    fprintf(fmips,"\t\tsyscall\n");

    addr = find_addr(midcode[midnum].dst);
    if(addr!=-1)//局部变量
        fprintf(fmips,"\t\tsw\t$v0\t-%d($fp)\n",addr);
    else//全局变量
    {
        fprintf(fmips,"\t\tla\t$t0\t%s\n", midcode[midnum].dst);
        fprintf(fmips,"\t\tsw\t$v0\t($t0)\n");
    }
}

void mips_ptf()
{
    int addr;
    //先输出字符串
    if(strcmp(midcode[midnum].scr1, "\0") != 0)
    {
         fprintf(fmips,"\t\tli\t$v0\t4\n");//v0=4:print string
         fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);
         fprintf(fmips,"\t\tmove\t$a0\t$t0\n");//a0=stringaddr
         fprintf(fmips,"\t\tsyscall\n");
    }
    if(strcmp(midcode[midnum].dst, "\0") == 0)
    {
        fprintf(fmips,"\t\tli\t$v0\t11\n");//v0=4:print character
        fprintf(fmips,"\t\tli\t$a0\t\'\\n'\n");//换行
        fprintf(fmips,"\t\tsyscall\n");
        return;
    }

    addr = find_addr(midcode[midnum].dst);
    if(addr!=-1)//非全局变量
        fprintf(fmips,"\t\tlw\t$a0\t-%d($fp)\n",addr);//a0 = dst
    else//全局变量
    {
        if(isdigit(midcode[midnum].dst[0])!=0 || midcode[midnum].dst=='-')//是数字
            fprintf(fmips,"\t\tli\t$a0\t%s\n", midcode[midnum].dst);
        else
        {
            fprintf(fmips,"\t\tla\t$t0\t%s\n", midcode[midnum].dst);
            fprintf(fmips,"\t\tlw\t$a0\t($t0)\n");
        }
    }
    if(strcmp(midcode[midnum].scr2, "int") == 0)
        fprintf(fmips,"\t\tli\t$v0\t1\n");//v0=1:print int
    else
        fprintf(fmips,"\t\tli\t$v0\t11\n");//v0=4:print character
    fprintf(fmips,"\t\tsyscall\n");
}

void mips_ret()
{
    int addr;
    if(strcmp(midcode[midnum].dst, "\0") != 0 )//return 还是return x
    {
        //v1是返回值
        if(isdigit(midcode[midnum].dst[0])==0 && midcode[midnum].dst[0]!='-')//不是数字
        {
            addr = find_addr(midcode[midnum].dst);
            if(addr!=-1)//非全局变量
                fprintf(fmips,"\t\tlw\t$v1\t-%d($fp)\n",addr);
            else//全局变量
            {
                fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].dst);//从全局变量中找到存进t0
                fprintf(fmips,"\t\tlw\t$v1\t($t0)\n");
            }
        }
        else
            fprintf(fmips,"\t\tli\t$v1\t%d\n",atoi(midcode[midnum].dst));//直接保存数字返回值
    }
    fprintf(fmips,"\t\tmove\t$t0\t$ra\n");//保存返回 t0=ra
    fprintf(fmips,"\t\tlw\t$ra\t($fp)\n");//ra = $fp
    fprintf(fmips,"\t\tmove\t$sp\t$fp\n");//sp = fp
    fprintf(fmips,"\t\tlw\t$fp\t4($fp)\n");//fp = prev fp
    fprintf(fmips,"\t\taddi\t$sp\t$sp\t%d\n",tmp_func_para*4+4);
    fprintf(fmips,"\t\tjr\t$t0\n");//跳到t0中的值
}

void mips_mret()
{
    fprintf(fmips,"\t\tli\t$v0\t10\n");//结束
    fprintf(fmips,"\t\tsyscall\n");
}

void mips_asmd()//t0=t0+t1
{
    int op;//0-add 1-sub 2-mul 3-div
    int addr1,addr2,addr3;
    if(strcmp(midcode[midnum].op, "+") == 0 )//先设置符号op
        op = 0;
    else if(strcmp(midcode[midnum].op, "-") == 0 )
        op = 1;
    else if(strcmp(midcode[midnum].op, "*") == 0 )
        op = 2;
    else if(strcmp(midcode[midnum].op, "/") == 0 )
        op = 3;
    //第一个操作数
    if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//不是数字
    {
        addr1 = find_addr(midcode[midnum].scr1);
        if(addr1 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)\n",addr1);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);//从全局变量中找到存进t0
            fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t0\t%s\n",midcode[midnum].scr1);
    //第二个操作数
    if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//不是数字
    {
        addr2 = find_addr(midcode[midnum].scr2);
        if(addr2 != -1)//不是全局变量
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
        else//全局变量
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//从全局变量中找到存进t1
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
    }
    else
        fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);

    if(op == 0)
        fprintf(fmips,"\t\tadd\t$t0\t$t0\t$t1\n");
    else if(op == 1)
        fprintf(fmips,"\t\tsub\t$t0\t$t0\t$t1\n");
    else if(op == 2)
        fprintf(fmips,"\t\tmul\t$t0\t$t0\t$t1\n");
    else if(op == 3)
        fprintf(fmips,"\t\tdiv\t$t0\t$t0\t$t1\n");

    //存进dst
    if(midcode[midnum].dst[0]=='$')//
    {
        insert_var(midcode[midnum].dst);
        addr3 = find_addr(midcode[midnum].dst);
        fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    }
    else//局部变量或全局变量
    {
        addr3 = find_addr(midcode[midnum].dst);
        if(addr3==-1)//全局变量
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].dst);//t1为dst的地址
            fprintf(fmips,"\t\tsw\t$t0\t($t1)\n");
        }
        else
            fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
    }
}
////////////////////////////////////初始化/////////////////////////////////////////////////
void init_mips()
{
    int i=0,j;
    int len = 0;
    char String[20];
    char add[10];
    fprintf(fmips,".data\n");
    for(j=0; strcmp(midcode[i].op, "const")==0; i++,j++)//全局const name: .word
    {
        strcpy(Global_Const[j].name, midcode[i].dst);
        strcpy(Global_Const[j].value, midcode[i].scr2);
        fprintf(fmips,"%s\t:\t.word\t%s\n",Global_Const[j].name,Global_Const[j].value);
        Global_Const_num++;
    }
    for(; strcmp(midcode[i].op, "var")==0; i++,j++)//全局var
    {
        strcpy(Global_Const[j].name, midcode[i].dst);
        if(strcmp("\0", midcode[i].scr2)==0)//非数组
        {
            strcpy(Global_Const[j].value, midcode[i].scr2);
            fprintf(fmips,"%s\t:\t.space\t4\n",Global_Const[j].name);
            Global_Const_num++;
        }
        else
        {
            strcpy(Global_Const[j].value, "\0");
            len = atoi(midcode[i].scr2);
            Global_Const_num++;
            fprintf(fmips,"%s\t:\t.space\t%d\n",Global_Const[j].name,len*4);
        }
    }
    Global_Const_num = i;
    midnum = i;
    //字符串保存
    for(;i<midconum;i++)
    {
        if(strcmp(midcode[i].op, "ptf")==0 && strcmp(midcode[i].scr1, "\0")!=0)//所有字符串
        {
            strcpy(String, "$String");
            itoa(Global_String_num,add,10);
            strcat(String, add);
            strcpy(Global_Const[j].value, midcode[i].scr1);
            strcpy(Global_Const[j].name, String);
            fprintf(fmips,"%s\t:\t.asciiz\t\"%s\"\n",Global_Const[j].name,midcode[i].scr1);
            Global_String_num ++;
            strcpy(midcode[i].scr1, String);//将字符串替换成一个变量StringXXX
            j++;
            Global_Const_num++;
        }
    }
    fprintf(fmips,".text\n");
    fprintf(fmips,".globl main\n");
    fprintf(fmips,"\t\taddi\t$fp\t$sp\t4\n");//fp = sp+4
    fprintf(fmips,"\t\taddi\t$sp\t$sp\t4\n");
    fprintf(fmips,"\t\tj\tmain\n");
    fprintf(fmips,"\t\tnop\n");
}

void clean_mips()
{
    int cl;
    midnum = 0;//转化完成的中间码数量
    Global_String_num = 0;//字符串的数量
    func_head_para = 0;//函数中声明了的参数数量
    tmp_func_para = 0;//当前函数声明了多少参量
    func_push_para = 0;//调用函数时已经传参i=func(a,b)的数量

    for(cl = 0; cl<Global_Const_num;cl++)
    {
        strcpy(Global_Const[cl].name,"\0");
        strcpy(Global_Const[cl].value,"\0");
    }

    for(cl = 0; cl<Local_Var_num;cl++)
    {
        strcpy(Local_Var[cl].name,"\0");
        strcpy(Local_Var[cl].funcname,"\0");
        Local_Var[cl].addr = 0;
    }

    Global_Const_num = 0;
    Local_Var_num = 0;
    offset = 0;
}
