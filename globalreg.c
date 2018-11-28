#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/*=============================�м���������==================================*/
typedef struct Count
{
    char name[100];
    int time;
    char funcname[100];
}Count;
Count count[3000];
char LABEL[200][20];
int LAEBELNUM=0;
int countnum = 0;
typedef struct Reg
{
    char name[100];
    char funcname[100];
}Reg;
Reg reg[8];

extern FILE *fmips;
extern int midnum;//ת����ɵ��м�������
extern int Global_String_num;//�ַ���������
extern int func_head_para;//�����������˵Ĳ�������
extern int tmp_func_para;//��ǰ���������˶��ٲ���
extern int func_push_para;//���ú���ʱ�Ѿ�����i=func(a,b)������
extern int midconum;
typedef struct symbol
{
    char name[50];
    int obj;    //��ʶ�����ࣺ����-0 ����-1 ����-2 ����-3
    int type;   //����-0 �ַ���-1 ������-2 void-3
    int value;  //������ֵ�����������ʱ0-���� 1-�ַ���
    int addr;   //����ջ�з��䴢�浥Ԫ����Ե�ַ
    int para;   //����ĳ��Ȼ��ߺ����Ĳ�������
}symbol;
extern symbol sym[1000];

typedef struct midco
{
    char op[10];	//������
	char scr1[128];	//��һ��������
	char scr2[64];	//�ڶ���������
    char dst[64];
}midco;
extern midco midcode[1000];

typedef struct opGlobal_const
{
    char name[32];
    char value[128];
    int reg;
}opGlobal_const;

typedef struct opLocal_var
{
    char name[32];
    char funcname[32];
    int addr;
    int reg;
}opLocal_var;

opLocal_var opLocal_Var[1024];
opGlobal_const opGlobal_Const[1024];

int opGlobal_Const_num = 0;
int opLocal_Var_num = 0;
extern int offset;
extern char FUNCNAME[32];

int declare = 0;
int globalnum = 0;
void count_midvar(char *name,char *functionname)
{
    int i;
    for(i = 0;i<countnum;i++)//�ֲ������ж�
    {
        if(strcmp(name,count[i].name)==0 && strcmp(functionname,count[i].funcname)==0)
        {
            count[i].time++;
            if(strcmp(functionname,"#global")==0)
                globalnum++;
            return;
        }
    }
    if(declare==0)
    {
        for(i = 0;i<globalnum;i++)//ȫ�ֱ����ж�
        {
            if(strcmp(name,count[i].name)==0)
            {
                count[i].time++;
                return;
            }
        }
    }

    //�����Ǿͽ�һ��
    if(strcmp(functionname,"#global")==0)
        globalnum++;
    strcpy(count[countnum].name,name);
    strcpy(count[countnum].funcname,functionname);
    count[countnum].time++;
    countnum++;
    return;
}

void count_times()
{
    int i,j;
    Count counttmp;
    char functionname[100];
    strcpy(functionname,"#global");
    for(i=0;i<midconum;i++)
    {
        if(strcmp(midcode[i].op,"const")==0||strcmp(midcode[i].op,"var")==0||strcmp(midcode[i].op,"para")==0)
            declare = 1;
        else
            declare = 0;
        if(strcmp(midcode[i].op,"func")==0)
            strcpy(functionname,midcode[i].dst);
        else if(strcmp(midcode[i].op,"label")==0 || strcmp(midcode[i].op,"j")==0
                ||strcmp(midcode[i].op,"mainret")==0 || strcmp(midcode[i].op,"bz")==0
                ||strcmp(midcode[i].op,"bnz")==0)
            ;
        else
        {

            if(strcmp(midcode[i].scr1,"\0")!=0 && strcmp(midcode[i].scr1,"int")!=0 && strcmp(midcode[i].scr1,"char")!=0)
            {
                if(isdigit(midcode[i].scr1[0])==0 && midcode[i].scr1[0]!='-')
                {
                    if(strcmp(midcode[i].scr2,"\0")!=0 && strcmp(midcode[i].op,"ass")==0)
                        continue;
                    count_midvar(midcode[i].scr1,functionname);
                }
            }
            if(strcmp(midcode[i].scr2,"\0")!=0 && strcmp(midcode[i].scr2,"int")!=0 && strcmp(midcode[i].scr2,"char")!=0)
            {
                if(isdigit(midcode[i].scr2[0])==0 && midcode[i].scr2[0]!='-')
                {
                    count_midvar(midcode[i].scr2,functionname);
                }
            }
            if(strcmp(midcode[i].dst,"\0")!=0 && strcmp(midcode[i].op,"assa")!=0)
                count_midvar(midcode[i].dst,functionname);
        }
    }
    for(j=0; j<countnum-1; j++)
    {
        for(i=0; i<countnum-j-1; i++)
        {
             if(count[i].time<count[i+1].time)
             {
                 counttmp=count[i+1];
                 count[i+1]=count[i];
                 count[i]=counttmp;
             }
         }
    }
    for(i=0;i<8;i++)
    {
        strcpy(reg[i].name,count[i].name);
        //printf("%s ",reg[i].name);
        strcpy(reg[i].funcname,count[i].funcname);
        //printf("%s\n",reg[i].funcname);
    }
}

int find_reg(char *NAME, char *FUNCNAME)
{
    int i=0,g;
    for(i = 0;i<8;i++)
    {
        if(strcmp(NAME,reg[i].name)==0 && strcmp(FUNCNAME,reg[i].funcname)==0)
            return i;
    }
    return -1;
}

int opfind_addr(char *NAME)
{
    int rt,i;
    for(i = 0; i<opLocal_Var_num;i++)
    {
        if(strcmp(NAME, opLocal_Var[i].name)==0 && strcmp(opLocal_Var[i].funcname,FUNCNAME)==0)
            return opLocal_Var[i].addr;
    }
    return -1;
}

int opfind_varreg(char *NAME)
{
    int i;
    for(i = 0; i<opLocal_Var_num;i++)
    {
        if(strcmp(NAME, opLocal_Var[i].name)==0 && strcmp(opLocal_Var[i].funcname,FUNCNAME)==0)
            return opLocal_Var[i].reg;
    }
    for(i = 0; i<opGlobal_Const_num;i++)
    {
        if(strcmp(NAME, opGlobal_Const[i].name)==0)
            return opGlobal_Const[i].reg;
    }
    return -1;
}

void opinsert_var(char *NAME)
{
    strcpy(opLocal_Var[opLocal_Var_num].name, NAME);
    opLocal_Var[opLocal_Var_num].addr = offset;
    opLocal_Var[opLocal_Var_num].reg = find_reg(NAME,FUNCNAME);
    strcpy(opLocal_Var[opLocal_Var_num].funcname,FUNCNAME);
    opLocal_Var_num++;
    offset = offset+4;
}
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////��������///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void opinit_mips();
void opclean_mips();
void opmips_func();
void opmips_call();
void opmips_para();
void opmips_push();
void opmips_assret();
void opmips_ass();
void opmips_assa();
void opmips_label();
void opmips_var();
void opmips_const();
void opmips_judge();
void opmips_branch();
void opmips_jmp();
void opmips_scf();
void opmips_ptf();
void opmips_ret();
void opmips_mret();
void opmips_asmd();//+-*/
/*======================================������=================================================*/
void global_reg()
{
    fmips=fopen("15231105_opregmips.txt","w");
    if(fmips==NULL)
    {
        printf("Can't open 15231105_opregmips.txt!\n");
        return;
    }
    count_times();
    strcpy(FUNCNAME,"\0");
    opinit_mips();

    midnum = 0;
    while(midnum<midconum)
    {
        if(strcmp(midcode[midnum].op, "func") == 0)
            opmips_func();
        else if(strcmp(midcode[midnum].op, "call") == 0)
            opmips_call();
        else if(strcmp(midcode[midnum].op, "para") == 0)
            opmips_para();
        else if(strcmp(midcode[midnum].op, "push") == 0)
            opmips_push();
        else if(strcmp(midcode[midnum].op, "assret") == 0)
            opmips_assret();
        else if(strcmp(midcode[midnum].op, "ass") == 0)
            opmips_ass();
        else if(strcmp(midcode[midnum].op, "assa") == 0)
            opmips_assa();
        else if(strcmp(midcode[midnum].op, "label") == 0 )
            opmips_label();
        else if(strcmp(midcode[midnum].op, "var") == 0 )
            opmips_var();
        else if(strcmp(midcode[midnum].op, "const") == 0 )
            opmips_const();
        else if(strcmp(midcode[midnum].op, "scf") == 0 )
            opmips_scf();
        else if(strcmp(midcode[midnum].op, "==") == 0 || strcmp(midcode[midnum].op, "!=")==0
                || strcmp(midcode[midnum].op, "<") == 0 || strcmp(midcode[midnum].op, "<=")==0
                || strcmp(midcode[midnum].op, ">") == 0 || strcmp(midcode[midnum].op, ">=")==0)
            opmips_judge();
        else if(strcmp(midcode[midnum].op, "bz") == 0 ||strcmp(midcode[midnum].op, "bnz") == 0)
            opmips_branch();
        else if(strcmp(midcode[midnum].op, "jmp") == 0 )
            opmips_jmp();
        else if(strcmp(midcode[midnum].op, "ptf") == 0 )
            opmips_ptf();
        else if(strcmp(midcode[midnum].op, "ret") == 0 )
            opmips_ret();
        else if(strcmp(midcode[midnum].op, "mainret") == 0 )
            opmips_mret();
        else if(strcmp(midcode[midnum].op, "+") == 0 || strcmp(midcode[midnum].op, "-")==0
                || strcmp(midcode[midnum].op, "*") == 0 || strcmp(midcode[midnum].op, "/")==0)
            opmips_asmd();
        midnum++;
    }

    fprintf(fmips,"\t\tli\t$v0\t10\n");//����
    fprintf(fmips,"\t\tsyscall\n");
    opclean_mips();
}

void opmips_func()
{
    int funcid;
    funcid = search_sym(midcode[midnum].dst,1);
    func_head_para = sym[funcid].para;
    tmp_func_para = func_head_para;
    offset = 8;//�޸�8 func ret
    fprintf(fmips,"%s:\n",midcode[midnum].dst);//����label
    strcpy(FUNCNAME,midcode[midnum].dst);//��ǰ�����ı�
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t8");//����ջָ��
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t%d\n",func_push_para*4);
}

void opmips_call()//���ú�������ret��Ӧ��
{
    int funcid;
    funcid = search_sym(midcode[midnum].dst,1);
    func_head_para = sym[funcid].para;
    offset = offset - func_head_para*4;
    func_push_para = 0;
    fprintf(fmips,"\t\tsw\t$fp\t($sp)");//����fp
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    fprintf(fmips,"\t\tsw\t$ra\t($sp)\n");//����ra
    fprintf(fmips,"\t\tmove\t$fp\t$sp\n");//fp = sp
    fprintf(fmips,"\t\tjal\t%s\n",midcode[midnum].dst);//��������
    fprintf(fmips,"\t\tnop\n");
}

void opmips_para()
{
    int addr,reg1;

    fprintf(fmips,"\t\tlw\t$t0\t%d($fp)",4+4*func_head_para);
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    opinsert_var(midcode[midnum].dst);
    addr = opfind_addr(midcode[midnum].dst);
    reg1 = opfind_varreg(midcode[midnum].dst);
    if(reg1>=0)
        fprintf(fmips,"\t\tmove\t$s%d\t$t0\n",reg1);
    else
        fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    func_head_para--;
}

void opmips_push()
{
    int addr,reg1;
    addr = opfind_addr(midcode[midnum].dst);
    reg1 = opfind_varreg(midcode[midnum].dst);
    if(reg1>=0)
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    else if(isdigit(midcode[midnum].dst[0])==0 && midcode[midnum].dst[0]!='-')//��������
    {
        if(addr != -1)//����ȫ�ֱ���
        {
            fprintf(fmips,"\t\tlw\t$t8\t-%d($fp)",addr);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }
        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t8\t%s",midcode[midnum].dst);//��ȫ�ֱ������ҵ����t8
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            fprintf(fmips,"\t\tlw\t$t8\t($t8)\n");
        }
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t8\t%s",midcode[midnum].dst);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }

    if(reg1>=0)
        fprintf(fmips,"\t\tsw\t$s%d\t($sp)\n",reg1);
    else
        fprintf(fmips,"\t\tsw\t$t8\t($sp)\n");//�¸���������ʼ�������Ӧλ��
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    offset+=4;
    func_push_para++;
}

void opmips_assret()
{
    int addr;
    opinsert_var(midcode[midnum].dst);//i=ret
    addr = opfind_addr(midcode[midnum].dst);
    fprintf(fmips,"\t\tsw\t$v1\t-%d($fp)",addr);//������ֵv1��
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
}

void opmips_ass()//a=b[i]
{
    int addr1,addr2,addr3;//add1=b-t0 addr2=i-t1 addr3=a-t2
    int isglobal=0;
    int reg1,reg2,reg3;
    char out1[10],out2[10],out3[10];
    char out1n[10],out2n[10],out3n[10];

    if(strcmp(midcode[midnum].scr2,"\0")==0)//�������鸳ֵ a=b
    {
        reg1 = opfind_varreg(midcode[midnum].scr1);
        if(reg1>=0)
        {
            strcpy(out1,"s");
            itoa(reg1,out1n,10);
            strcat(out1,out1n);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }
        else if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//��������
        {
            addr1 = opfind_addr(midcode[midnum].scr1);
            if(addr1 != -1)//����ȫ�ֱ���
            {
                fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)",addr1);
                fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            }

            else//ȫ�ֱ���
            {
                fprintf(fmips,"\t\tla\t$t0\t%s",midcode[midnum].scr1);//��ȫ�ֱ������ҵ����t0
                fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
                fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
            }
            strcpy(out1,"t0");
        }
        else
        {
            fprintf(fmips,"\t\tli\t$t0\t%s",midcode[midnum].scr1);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            strcpy(out1,"t0");
        }

        reg2 = opfind_varreg(midcode[midnum].dst);
        if(reg2>=0)
        {
            fprintf(fmips,"\t\tmove\t$s%d\t$%s\n",reg2,out1);
        }

        else if(midcode[midnum].dst[0]=='$')//
        {
            opinsert_var(midcode[midnum].dst);
            addr3 = opfind_addr(midcode[midnum].dst);
            fprintf(fmips,"\t\tsw\t$%s\t-%d($fp)\n",out1,addr3);
            fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
        }
        else
        {
            addr3 = opfind_addr(midcode[midnum].dst);
            if(addr3 != -1)//����ȫ�ֱ���
                fprintf(fmips,"\t\tsw\t$%s\t-%d($fp)\n",out1,addr3);
            else//ȫ�ֱ���
            {
                fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].dst);//��ȫ�ֱ������ҵ����t2
                fprintf(fmips,"\t\tsw\t$%s\t($t2)\n",out1);
            }
        }
    }

    else
    {
        //b��λ��
        addr1 = opfind_addr(midcode[midnum].scr1);
        if(addr1 != -1)//����ȫ�ֱ���
        {
            isglobal = 0;
            fprintf(fmips,"\t\tli\t$t0\t-%d",addr1);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            fprintf(fmips,"\t\tadd\t$t0\t$t0\t$fp\n");
        }
        else//ȫ�ֱ���
        {
            isglobal = 1;
            fprintf(fmips,"\t\tla\t$t0\t%s",midcode[midnum].scr1);//��ȫ�ֱ������ҵ����t0
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }
        //����ƫ����t1
        addr2 = opfind_addr(midcode[midnum].scr2);
        reg2 = opfind_varreg(midcode[midnum].scr2);
        if(reg2>=0)
        {
            strcpy(out2,"s");
            itoa(reg2,out2n,10);
            strcat(out2,out2n);
        }
        else if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//��������
        {
            if(addr2 != -1)//����ȫ�ֱ���
                fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
            else//ȫ�ֱ���
            {
                fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//��ȫ�ֱ������ҵ����t1
                fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
            }
            strcpy(out2,"t1");
        }
        else
        {
            fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);
            strcpy(out2,"t1");
        }
        if(isglobal == 1)//ȫ�ֱ���
            fprintf(fmips,"\t\tmul\t$t1\t$%s\t4\n",out2);
        else
            fprintf(fmips,"\t\tmul\t$t1\t$%s\t-4\n",out2);
        fprintf(fmips,"\t\tadd\t$t0\t$t0\t$t1\n");
        fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        opinsert_var(midcode[midnum].dst);
        addr2 = opfind_addr(midcode[midnum].dst);
        reg3 = opfind_varreg(midcode[midnum].dst);
        if(reg3>=0)
        {
            fprintf(fmips,"\t\tmove\t$s%d\t$t0\n",reg3);
        }
        else
            fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr2);
    }
}

void opmips_assa()//a[i] = b
{
    int addr1,addr2,addr3;//addr1-b t0 addr2-i t1 addr3-a t2
    int reg1,reg2,reg3;
    char out1[10],out2[10],out3[10];
    char out1n[10],out2n[10],out3n[10];

    addr3 = opfind_addr(midcode[midnum].dst);

    reg1 = opfind_varreg(midcode[midnum].scr1);
    if(reg1>=0)
    {
        strcpy(out1,"s");
        itoa(reg1,out1n,10);
        strcat(out1,out1n);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }
    else if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//��������
    {
        addr1 = opfind_addr(midcode[midnum].scr1);
        if(addr1 != -1)//����ȫ�ֱ���
        {
            fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)",addr1);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }

        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t0\t%s",midcode[midnum].scr1);//��ȫ�ֱ������ҵ����t0
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        }
        strcpy(out1,"t0");
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t0\t%s",midcode[midnum].scr1);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        strcpy(out1,"t0");
    }


    reg2 = opfind_varreg(midcode[midnum].scr2);
    if(reg2>=0)
    {
        strcpy(out2,"s");
        itoa(reg2,out2n,10);
        strcat(out2,out2n);
    }
    else if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//��������
    {
        addr2 = opfind_addr(midcode[midnum].scr2);
        if(addr2 != -1)//����ȫ�ֱ���
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//��ȫ�ֱ������ҵ����t1
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
        strcpy(out2,"t1");
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);
        strcpy(out2,"t1");
    }

    if(addr3 != -1)//�ֲ�����
    {
        fprintf(fmips,"\t\tmul\t$t1\t$%s\t-4\n",out2);
        fprintf(fmips,"\t\tli\t$t2\t-%d\n",addr3);
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$t2\n");
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$fp\n");
    }
    else
    {
        fprintf(fmips,"\t\tmul\t$t1\t$%s\t4\n",out2);
        fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].dst);
        fprintf(fmips,"\t\tadd\t$t1\t$t1\t$t2\n");
    }

    fprintf(fmips,"\t\tsw\t$%s\t($t1)\n",out1);
}

void opmips_var()
{
    int arraylen;
    strcpy(opLocal_Var[opLocal_Var_num].name,midcode[midnum].dst);
    opLocal_Var[opLocal_Var_num].addr = offset;
    strcpy(opLocal_Var[opLocal_Var_num].funcname,FUNCNAME);

    if(strcmp(midcode[midnum].scr2,"\0")==0)//��������
    {
        opLocal_Var[opLocal_Var_num].reg = find_reg(midcode[midnum].dst,FUNCNAME);
        offset = offset + 4;
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    }
    else//����
    {
        opLocal_Var[opLocal_Var_num].reg = -1;
        arraylen = atoi(midcode[midnum].scr2);
        offset = offset + 4*arraylen;
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t%d\n", 4*arraylen);//sp-4*arraylen
    }
    opLocal_Var_num++;
}

void opmips_const()
{
    int arraylen;
    strcpy(opLocal_Var[opLocal_Var_num].name,midcode[midnum].dst);
    strcpy(opLocal_Var[opLocal_Var_num].funcname,FUNCNAME);
    opLocal_Var[opLocal_Var_num].addr = offset;
    opLocal_Var[opLocal_Var_num].reg = find_reg(midcode[midnum].dst,FUNCNAME);
    opLocal_Var_num++;
    //��ֵ
    fprintf(fmips,"\t\tli\t$t0\t%s\n",midcode[midnum].scr2);
    fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",offset);
    fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");//sp-4
    offset += 4;
}

void opmips_label()
{
    fprintf(fmips,"%s:\n",midcode[midnum].dst);//����label
}

void opmips_judge()
{
    int op;//0:== 1:!= 2:> 3:>= 4:< 5:<=
    int reg1,reg2;
    int addr1,addr2;
    char out1[10],out2[10],out1n[10],out2n[10];
    if(strcmp(midcode[midnum].op, "==") == 0 )//�����÷���op
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
    /*-----------------------------------������1------------------------------*/
    reg1 = opfind_varreg(midcode[midnum].scr1);
    if(reg1>=0)
    {
        strcpy(out1,"s");
        itoa(reg1,out1n,10);
        strcat(out1,out1n);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }
    else if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//��������
    {
        addr1 = opfind_addr(midcode[midnum].scr1);
        if(addr1 != -1)//����ȫ�ֱ���
        {
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)",addr1);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }

        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t1\t%s",midcode[midnum].scr1);//��ȫ�ֱ������ҵ����t0
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
        strcpy(out1,"t1");
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t1\t%s",midcode[midnum].scr1);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        strcpy(out1,"t1");
    }



    /*-----------------------------------������2------------------------------*/
    reg2 = opfind_varreg(midcode[midnum].scr2);
    if(reg2>=0)
    {
        strcpy(out2,"s");
        itoa(reg2,out2n,10);
        strcat(out2,out2n);
    }
    else if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//��������,scr2Ϊ����
    {
        addr2 = opfind_addr(midcode[midnum].scr2);
        if(addr2 != -1)//����ȫ�ֱ���
            fprintf(fmips,"\t\tlw\t$t2\t-%d($fp)\n",addr2);
        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t2\t%s\n",midcode[midnum].scr2);//��ȫ�ֱ������ҵ����t1
            fprintf(fmips,"\t\tlw\t$t2\t($t2)\n");
        }
        strcpy(out2,"t2");
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t2\t%s\n",midcode[midnum].scr2);
        strcpy(out2,"t2");
    }

    if(op == 0)//==
        fprintf(fmips,"\t\tseq\t$t0\t$%s\t$%s\n",out1,out2);//�����t0��1
    if(op == 1)//!=
        fprintf(fmips,"\t\tsne\t$t0\t$%s\t$%s\n",out1,out2);
    if(op == 2)//>
        fprintf(fmips,"\t\tsgt\t$t0\t$%s\t$%s\n",out1,out2);
    if(op == 3)//>=
        fprintf(fmips,"\t\tsge\t$t0\t$%s\t$%s\n",out1,out2);
    if(op == 4)//<
        fprintf(fmips,"\t\tslt\t$t0\t$%s\t$%s\n",out1,out2);
    if(op == 5)//<=
        fprintf(fmips,"\t\tsle\t$t0\t$%s\t$%s\n",out1,out2);

}
void opmips_branch()
{
    int branch;//1-����1��ת 0-����0��ת
    if(strcmp(midcode[midnum].op, "bz") == 0)//1��ת bz������������ת
    {
        fprintf(fmips,"\t\tbeq\t$t0\t0\t%s",midcode[midnum].dst);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }
    else
    {
        fprintf(fmips,"\t\tbeq\t$t0\t1\t%s",midcode[midnum].dst);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }
}

void opmips_jmp()
{
    fprintf(fmips,"\t\tj\t%s",midcode[midnum].dst);
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tnop\n");
}

void opmips_scf()
{
    int addr,reg1;
    if(strcmp(midcode[midnum].scr1, "int") == 0)
        fprintf(fmips,"\t\tli\t$v0\t5\n");//v0=5:read int
    else
        fprintf(fmips,"\t\tli\t$v0\t12\n");//v0=4:read character
    fprintf(fmips,"\t\tsyscall");
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);

    reg1 = opfind_varreg(midcode[midnum].dst);
    addr = opfind_addr(midcode[midnum].dst);
    if(reg1>=0)
    {
        fprintf(fmips,"\t\tmove\t$s%d\t$v0\n",reg1);
    }
    else if(addr!=-1)//�ֲ�����
        fprintf(fmips,"\t\tsw\t$v0\t-%d($fp)\n",addr);
    else//ȫ�ֱ���
    {
        fprintf(fmips,"\t\tla\t$t0\t%s\n", midcode[midnum].dst);
        fprintf(fmips,"\t\tsw\t$v0\t($t0)\n");
    }
}

void opmips_ptf()
{
    int addr,reg1;

    //������ַ���
    if(strcmp(midcode[midnum].scr1, "\0") != 0)
    {
         fprintf(fmips,"\t\tli\t$v0\t4");//v0=4:print string
         fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
         fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].scr1);
         fprintf(fmips,"\t\tmove\t$a0\t$t0\n");//a0=stringaddr
         fprintf(fmips,"\t\tsyscall\n");
    }
    if(strcmp(midcode[midnum].dst, "\0") == 0)
    {
        fprintf(fmips,"\t\tli\t$v0\t11\n");//v0=4:print character
        fprintf(fmips,"\t\tli\t$a0\t\'\\n'\n");//����
        fprintf(fmips,"\t\tsyscall\n");
        return;
    }

    addr = opfind_addr(midcode[midnum].dst);
    reg1 = opfind_varreg(midcode[midnum].dst);
    if(reg1>=0)
    {
        fprintf(fmips,"\t\tmove\t$a0\t$s%d\n", reg1);
    }
    else if(addr!=-1)//��ȫ�ֱ���
        fprintf(fmips,"\t\tlw\t$a0\t-%d($fp)\n",addr);//a0 = dst
    else//ȫ�ֱ���
    {
        if(isdigit(midcode[midnum].dst[0])!=0 || midcode[midnum].dst=='-')//������
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

    fprintf(fmips,"\t\tli\t$v0\t11\n");//v0=4:print character
    fprintf(fmips,"\t\tli\t$a0\t\'\\n'\n");//����
    fprintf(fmips,"\t\tsyscall\n");
}

void opmips_ret()
{
    int addr,reg1;


    if(strcmp(midcode[midnum].dst, "\0") != 0 )//return ����return x
    {
        //v1�Ƿ���ֵ
        reg1 = opfind_varreg(midcode[midnum].dst);
        if(reg1>=0)
        {
            fprintf(fmips,"\t\tmove\t$v1\t$s%d\n", reg1);
        }
        else if(isdigit(midcode[midnum].dst[0])==0 && midcode[midnum].dst[0]!='-')//��������
        {
            addr = opfind_addr(midcode[midnum].dst);
            if(addr!=-1)//��ȫ�ֱ���
                fprintf(fmips,"\t\tlw\t$v1\t-%d($fp)\n",addr);
            else//ȫ�ֱ���
            {
                fprintf(fmips,"\t\tla\t$t0\t%s\n",midcode[midnum].dst);//��ȫ�ֱ������ҵ����t0
                fprintf(fmips,"\t\tlw\t$v1\t($t0)\n");
            }
        }
        else
            fprintf(fmips,"\t\tli\t$v1\t%d\n",atoi(midcode[midnum].dst));//ֱ�ӱ������ַ���ֵ
    }
    fprintf(fmips,"\t\tmove\t$t0\t$ra");//���淵�� t0=ra
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tlw\t$ra\t($fp)\n");//ra = $fp
    fprintf(fmips,"\t\tmove\t$sp\t$fp\n");//sp = fp
    fprintf(fmips,"\t\tlw\t$fp\t4($fp)\n");//fp = prev fp
    fprintf(fmips,"\t\taddi\t$sp\t$sp\t%d\n",tmp_func_para*4+4);
    fprintf(fmips,"\t\tjr\t$t0\n");//����t0�е�ֵ
}

void opmips_mret()
{
    fprintf(fmips,"\t\tli\t$v0\t10");//����
    fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    fprintf(fmips,"\t\tsyscall\n");
}

void opmips_asmd()//t0=t0+t1
{
    int op;//0-add 1-sub 2-mul 3-div
    int reg1,reg2,reg3;
    char out1[10],out2[10],out3[10];
    char out1n[10],out2n[10],out3n[10];

    int addr1,addr2,addr3;
    if(strcmp(midcode[midnum].op, "+") == 0 )//�����÷���op
        op = 0;
    else if(strcmp(midcode[midnum].op, "-") == 0 )
        op = 1;
    else if(strcmp(midcode[midnum].op, "*") == 0 )
        op = 2;
    else if(strcmp(midcode[midnum].op, "/") == 0 )
        op = 3;

    //��һ��������
    reg1 = opfind_varreg(midcode[midnum].scr1);
    if(reg1>=0)
    {
        strcpy(out1,"s");
        itoa(reg1,out1n,10);
        strcat(out1,out1n);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
    }
    else if(isdigit(midcode[midnum].scr1[0])==0 && midcode[midnum].scr1[0]!='-')//��������
    {
        addr1 = opfind_addr(midcode[midnum].scr1);
        if(addr1 != -1)//����ȫ�ֱ���
        {
            fprintf(fmips,"\t\tlw\t$t0\t-%d($fp)",addr1);
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        }
        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t0\t%s",midcode[midnum].scr1);//��ȫ�ֱ������ҵ����t0
            fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
            fprintf(fmips,"\t\tlw\t$t0\t($t0)\n");
        }
        strcpy(out1,"t0");
    }
    else
    {
        fprintf(fmips,"\t\tli\t$t0\t%s",midcode[midnum].scr1);
        fprintf(fmips,"#%s %s %s %s\n",midcode[midnum].op,midcode[midnum].scr1,midcode[midnum].scr2,midcode[midnum].dst);
        strcpy(out1,"t0");
    }

    //�ڶ���������
    reg2 = opfind_varreg(midcode[midnum].scr2);
    if(reg2>=0)
    {
        strcpy(out2,"s");
        itoa(reg2,out2n,10);
        strcat(out2,out2n);
    }
    else if(isdigit(midcode[midnum].scr2[0])==0 && midcode[midnum].scr2[0]!='-')//��������
    {
        addr2 = opfind_addr(midcode[midnum].scr2);
        if(addr2 != -1)//����ȫ�ֱ���
            fprintf(fmips,"\t\tlw\t$t1\t-%d($fp)\n",addr2);
        else//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].scr2);//��ȫ�ֱ������ҵ����t1
            fprintf(fmips,"\t\tlw\t$t1\t($t1)\n");
        }
        strcpy(out2,"t1");
    }
    else
    {
        strcpy(out2,"t1");
        fprintf(fmips,"\t\tli\t$t1\t%s\n",midcode[midnum].scr2);
    }

    reg3 = opfind_varreg(midcode[midnum].dst);
    if(reg3>=0)
    {
        strcpy(out3,"s");
        itoa(reg3,out3n,10);
        strcat(out3,out3n);
    }
    else
        strcpy(out3,"t0");

    if(op == 0)
        fprintf(fmips,"\t\tadd\t$%s\t$%s\t$%s\n",out3,out1,out2);
    else if(op == 1)
        fprintf(fmips,"\t\tsub\t$%s\t$%s\t$%s\n",out3,out1,out2);
    else if(op == 2)
        fprintf(fmips,"\t\tmul\t$%s\t$%s\t$%s\n",out3,out1,out2);
    else if(op == 3)
        fprintf(fmips,"\t\tdiv\t$%s\t$%s\t$%s\n",out3,out1,out2);

    //���dst
    if(reg3>=0)
    {
        ;
    }
    else if(midcode[midnum].dst[0]=='$')//
    {
        opinsert_var(midcode[midnum].dst);
        addr3 = opfind_addr(midcode[midnum].dst);
        fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
        fprintf(fmips,"\t\tsubi\t$sp\t$sp\t4\n");
    }
    else//�ֲ�������ȫ�ֱ���
    {
        addr3 = opfind_addr(midcode[midnum].dst);
        if(addr3==-1)//ȫ�ֱ���
        {
            fprintf(fmips,"\t\tla\t$t1\t%s\n",midcode[midnum].dst);//t1Ϊdst�ĵ�ַ
            fprintf(fmips,"\t\tsw\t$t0\t($t1)\n");
        }
        else
            fprintf(fmips,"\t\tsw\t$t0\t-%d($fp)\n",addr3);
    }
}
////////////////////////////////////��ʼ��/////////////////////////////////////////////////
void opinit_mips()
{
    int i=0,j;
    int len = 0;
    char String[20];
    char add[10];
    fprintf(fmips,".data\n");
    for(j=0; strcmp(midcode[i].op, "const")==0; i++,j++)//ȫ��const name: .word
    {
        strcpy(opGlobal_Const[j].name, midcode[i].dst);
        strcpy(opGlobal_Const[j].value, midcode[i].scr2);
        if(find_reg(opGlobal_Const[j].name,"#global")>=0)
            opGlobal_Const[j].reg = find_reg(opGlobal_Const[j].name,"#global");
        else
            opGlobal_Const[j].reg = -1;
        fprintf(fmips,"%s\t:\t.word\t%s\n",opGlobal_Const[j].name,opGlobal_Const[j].value);
        opGlobal_Const_num++;
    }
    for(; strcmp(midcode[i].op, "var")==0; i++,j++)//ȫ��var
    {
        strcpy(opGlobal_Const[j].name, midcode[i].dst);
        if(strcmp("\0", midcode[i].scr2)==0)//������
        {
            if(find_reg(opGlobal_Const[j].name,"#global")>=0)
                opGlobal_Const[j].reg = find_reg(opGlobal_Const[j].name,"#global");
            else
                opGlobal_Const[j].reg = -1;
            strcpy(opGlobal_Const[j].value, midcode[i].scr2);
            fprintf(fmips,"%s\t:\t.space\t4\n",opGlobal_Const[j].name);
            opGlobal_Const_num++;
        }
        else
        {
            strcpy(opGlobal_Const[j].value, "\0");
            opGlobal_Const[j].reg = -1;
            len = atoi(midcode[i].scr2);
            opGlobal_Const_num++;
            fprintf(fmips,"%s\t:\t.space\t%d\n",opGlobal_Const[j].name,len*4);
        }
    }
    opGlobal_Const_num = i;
    midnum = i;
    //�ַ�������
    for(;i<midconum;i++)
    {
        if(strcmp(midcode[i].op, "ptf")==0 && strcmp(midcode[i].scr1, "\0")!=0)//�����ַ���
        {
            strcpy(String, "$String");
            itoa(Global_String_num,add,10);
            strcat(String, add);
            strcpy(opGlobal_Const[j].value, midcode[i].scr1);
            strcpy(opGlobal_Const[j].name, String);
            opGlobal_Const[j].reg = 9;
            fprintf(fmips,"%s\t:\t.asciiz\t\"%s\"\n",opGlobal_Const[j].name,midcode[i].scr1);
            Global_String_num ++;
            strcpy(midcode[i].scr1, String);//���ַ����滻��һ������StringXXX
            j++;
            opGlobal_Const_num++;
        }
    }
    fprintf(fmips,".text\n");
    fprintf(fmips,".globl main\n");
    fprintf(fmips,"\t\taddi\t$fp\t$sp\t4\n");//fp = sp+4
    fprintf(fmips,"\t\taddi\t$sp\t$sp\t4\n");
    fprintf(fmips,"\t\tj\tmain\n");
    fprintf(fmips,"\t\tnop\n");
}

void opclean_mips()
{
    int cl;
    midnum = 0;//ת����ɵ��м�������
    Global_String_num = 0;//�ַ���������
    func_head_para = 0;//�����������˵Ĳ�������
    tmp_func_para = 0;//��ǰ���������˶��ٲ���
    func_push_para = 0;//���ú���ʱ�Ѿ�����i=func(a,b)������

    for(cl = 0; cl<opGlobal_Const_num;cl++)
    {
        strcpy(opGlobal_Const[cl].name,"\0");
        strcpy(opGlobal_Const[cl].value,"\0");
        opGlobal_Const[cl].reg = -1;
    }

    for(cl = 0; cl<opLocal_Var_num;cl++)
    {
        strcpy(opLocal_Var[cl].name,"\0");
        strcpy(opLocal_Var[cl].funcname,"\0");
        opLocal_Var[cl].addr = 0;
        opLocal_Var[cl].reg = -1;
    }

    opGlobal_Const_num = 0;
    opLocal_Var_num = 0;
    offset = 0;
}
