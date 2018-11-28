#ifndef MIPSHEAD_H_INCLUDED
#define MIPSHEAD_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/*=============================中间代码操作符==================================*/
char mspace[] = " ";
char mconst[] = "const";
char mvar[] = "var";
char mfunc[] = "func";
char mmain[] = "main";
char mint[] = "int";
char mvoid[] = "void";
char mchar[] = "char";
char mass[] = "ass";
char massa[] = "assa";
char mpara[] = "para";
char mpush[] = "push";
char mcall[] = "call";
char mret[] = "ret";
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

#endif // MIPSHEAD_H_INCLUDED
