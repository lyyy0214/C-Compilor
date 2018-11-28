#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

/*------------保留字----------*/
#define CONST 0
#define INT 1
#define CHAR 2
#define VOID 3
#define MAIN 4
#define IF 5
#define DO 6
#define WHILE 7
#define SWITCH 8
#define CASE 9
#define SCANF 10
#define PRINTF 11
#define RETURN 12

/*------------非符号----------*/
#define IDENTITY 13
#define NUMBER 14
#define ZERO 15
#define STRING 16
#define CHARACTER 17

/*-------------括号------------*/
#define LPAR 18 //(
#define RPAR 19 //)
#define LBRACE 20   //{
#define RBRACE 21   //}
#define LSQB 22 //[
#define RSQB 23 //]

/*-------------运算符号------------*/
#define ADD 24
#define SUB 25
#define MUL 26
#define DIV 27
#define ASS 28  //=

/*-------------关系符号------------*/
#define LEQ 29  //<=
#define LE 30   //<
#define GTQ 31  //>=
#define GT 32   //>
#define EQU 33  //==
#define NEQ 34  //!=

/*-------------分界符号------------*/
#define SEMICOL 35  //;
#define COLON 36    //:
#define COMMA 37    //,

/*-------------词法分析------------*/
#define vmax 100
FILE *fin,*fout;
//void error(int pos, int errortype);

#endif // TYPE_H_INCLUDED
