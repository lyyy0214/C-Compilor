#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
FILE *fdag;
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
opmidco opmidcode[1000];

typedef struct BasicBlock{//������ṹ
	int startnumber; //��ʼԭ�м�������
	int endnumber;//����ԭ�м�������
}BasicBlock;
BasicBlock basicblock[1000];


typedef struct dagNode{
	int isExported;//�ж��Ƿ��ӡ��
	int tablenumber;//�ڽڵ���еĽڵ��
	int leftNumber;//����dag��
	int rightNumber;//�Һ���dag��
	char name[100];//�ڵ�����
	int mid;//��Ϊ0��ʱ�򣬱���ΪҶ�ڵ㣬��Ϊ1��ʱ�򣬱���Ϊ�м�ڵ㡣
}dagNode;
dagNode dagnode[1000];

typedef struct Ntable{
	int dagNumber;//�ýڵ���dagͼ�е����
	char nodeName[100];
	int nodeoff;//�ýڵ��λ�Ƶ����
}Ntable;
Ntable nodetable[1000];

int opmidcodenum = 0;//���ɵ��Ż���Ĵ�������
int basicblocknum = 0;//�����������
int dagnodenum = 0;//dagͼ�еĽڵ�����
int nodetablenum = 0;//�ڵ���еĽڵ�����
int deriveline[2048];//�����Ľڵ��� ��������,������dagͼ�е����
int derivelinenum = 0;

void make_block();//���ֻ�����
void produce_dag();//����dagͼ
int find_node(char *name);//�ڽڵ�����ҽڵ�
int find_dag(int l, int r);//��dagͼ�����м�ڵ�
void derive_dag();//����dagͼ
int father_derived(int child);//�鿴���ڵ��Ƿ������
int choose_derive();//ѡ��һ���ڵ㵼��
int var_derive(int dagnum, int start);//�����ı���
void combine_ass();
void finish();//����һ�λ�����Ļ���

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

void make_block()//���ֻ�����
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
            basicblock[basicblocknum].startnumber = i;//��������ʽ����ʼ
            while(strcmp(midcode[i].op,"+")==0 || strcmp(midcode[i].op,"-")==0 || strcmp(midcode[i].op,"*")==0 ||
                  strcmp(midcode[i].op,"/")==0 || strcmp(midcode[i].op,"ass")==0 || strcmp(midcode[i].op,"assa")==0)
            {
                i++;
            }
            basicblock[basicblocknum].endnumber = i;//�����������i-1
            produce_dag();//����dagͼ
            derive_dag();
            combine_ass;
            finish();
            basicblocknum++;
        }
    }
}

void produce_dag()
{
    int i,l,r,d,t;//d��dagͼ�е���� t�ڽڵ���е����
    char array[100];
    int rt,j;
    i = basicblock[basicblocknum].startnumber;
    while(i<basicblock[basicblocknum].endnumber)//��ֵ�����Ҫ��������
    {
        //��ڵ�
        l = find_node(midcode[i].scr1);
        if(l<0)//��һ�����������ڽڵ����,�ڵ��&dagͼ�м����½ڵ�
        {
            //�ڵ����
            strcpy(nodetable[nodetablenum].nodeName,midcode[i].scr1);
            nodetable[nodetablenum].dagNumber = dagnodenum;
            nodetable[nodetablenum].nodeoff = -1;
            //dagͼ����
            strcpy(dagnode[dagnodenum].name,midcode[i].scr1);
            dagnode[dagnodenum].mid = 0;
            dagnode[dagnodenum].leftNumber = -1;
            dagnode[dagnodenum].rightNumber = -1;
            dagnode[dagnodenum].isExported = 0;
            dagnode[dagnodenum].tablenumber = nodetablenum;
            l = dagnodenum;

            nodetablenum++,dagnodenum++;
        }
        else//�ڵ���д��ڸýڵ�
        {
            //���������Ԫ�أ����ٴ�Ѱ�Ҹ�׼ȷ�Ľڵ�
            if(strcmp(midcode[i].op,"ass")==0 && strcmp(midcode[i].scr2,"\0")!=0)
            {
                j = l;//jɨ��һ��nodetable
                while(j<nodetablenum)
                {
                    if(strcmp(nodetable[j].nodeName,midcode[i].scr1)==0)//��������ͬ
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

        //�ҽڵ�
        if(strcmp(midcode[i].scr2,"\0")!=0)
        {
            r = find_node(midcode[i].scr2);
            rt = r;//rt�����Һ����ڽڵ���еĽڵ�
            if(r<0)//��2�����������ڽڵ����,�ڵ��&dagͼ�м����½ڵ�
            {
                //�ڵ����
                strcpy(nodetable[nodetablenum].nodeName,midcode[i].scr2);
                nodetable[nodetablenum].dagNumber = dagnodenum;
                nodetable[nodetablenum].nodeoff = -1;
                //dagͼ����
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
            else//�ڵ���д��ڸýڵ�
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
        if(t<0)//dst�ڵ���Ҫ����
        {
            strcpy(nodetable[nodetablenum].nodeName,midcode[i].dst);
            t = nodetablenum;
            nodetable[nodetablenum].nodeoff = rt;
            nodetablenum++;
        }
        else if(strcmp(midcode[i].op,"assa")==0)//���鸳ֵ��������ͬ������
        {
            strcpy(array,nodetable[nodetable[t].nodeoff].nodeName);
            if(strcmp(array,midcode[i].scr2)==0)
                ;
            else
            {
                strcpy(nodetable[nodetablenum].nodeName,midcode[i].dst);
                t = nodetablenum;
                nodetable[nodetablenum].nodeoff = rt;//scr2������λ���ڽڵ���е����
                nodetablenum++;
            }
        }

        d = find_dag(l,r);//����dagͼ�п�����û��dst,ͬʱ�鿴�����Ƿ���ͬ
        if(d<0||strcmp(midcode[i].op,dagnode[d].name)!=0||(strcmp(midcode[i].op,"ass")!=0&&strcmp(midcode[i].scr2,"\0")==0))
        {
            //dagͼ�н����м�ڵ�
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
               &&midcode[i].scr1[0]!='$')//�������ӽڵ�
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
            else if( dagnode[d].mid == 0)//Ҷ�ڵ���һ�����ϵı�������Ҫ����
            {
                dagnode[d].mid = 2;
            }
        }
        nodetable[t].dagNumber = d;
        if(dagnode[d].tablenumber == -1)//�½�����dagͼ�еĽڵ�
            dagnode[d].tablenumber = t;
        else//֮ǰ�������������ܲ��ܰ���ʱ�����滻����ͨ����
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

    for(i = 0; i < len; i++)//���ε���i
    {
        derive_target = reversed[i];
        dagnode[derive_target].isExported = 1;
        l = dagnode[derive_target].leftNumber;//dagͼ�����ӽڵ���dagͼ�е����
        r = dagnode[derive_target].rightNumber;
        d = dagnode[derive_target].tablenumber;
        if(l>=0 && r>=0)
        {
            l = dagnode[l].tablenumber;
            r = dagnode[r].tablenumber;
            strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//��
            strcpy(opmidcode[opmidcodenum].scr2,nodetable[r].nodeName);//��
            strcpy(opmidcode[opmidcodenum].dst,nodetable[d].nodeName);
            strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
            opmidcodenum++;
            j = 0;
            while(var_derive(derive_target,j)>=0)//�����ڵ���������ķ���ʱ����
            {
                var_target = var_derive(derive_target,j);
                strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//��
                strcpy(opmidcode[opmidcodenum].scr2,nodetable[r].nodeName);//��
                strcpy(opmidcode[opmidcodenum].dst,nodetable[var_target].nodeName);
                strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
                opmidcodenum++;
                j = var_target+1;
            }
        }
        else if(r<0&&l>=0)//a=b
        {
            l = dagnode[l].tablenumber;
            strcpy(opmidcode[opmidcodenum].scr1,nodetable[l].nodeName);//��
            strcpy(opmidcode[opmidcodenum].scr2,"\0");//��
            strcpy(opmidcode[opmidcodenum].dst,nodetable[d].nodeName);
            strcpy(opmidcode[opmidcodenum].op,dagnode[derive_target].name);
            opmidcodenum++;
        }
        else//�ڵ�������������
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
                    strcpy(opmidcode[opmidcodenum].scr1,nodetable[first].nodeName);//��
                    strcpy(opmidcode[opmidcodenum].scr2,"\0");//��
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
                i++;//һ����������
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
        if(dagnode[i].mid>0 && father_derived(i)==1 && dagnode[i].isExported==0)//����Ҷ�ӽڵ��Ҹ��ڵ������
            return i;
        i--;
    }
    return -1;
}

int father_derived(int child)//��child�ĸ��ڵ��Ƿ񶼵�����
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
        if(nodetable[i].dagNumber == dagnum && dagnode[dagnum].tablenumber!=i)//���ж϶�Ӧ��dagͼ�����dagnum
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
        else if(i==l&&r==-1)//��ͨ��ֵ�����ڵ�Ϊ�ýڵ㣬�ҽڵ�Ϊ-1
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

