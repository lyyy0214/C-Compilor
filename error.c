#include <stdio.h>
#include <stdlib.h>

void error(int pos, int errortype)
{
    if(pos>=0)
        printf("on line%d ,",pos);
    switch (errortype)
    {
        case 0:{printf("word error! wrong type in STRING!\n");break;}
        case 1:{printf("word error! should be one character in CHAR!\n");break;}
        case 2:{printf("word error! wrong type in CHAR!\n");break;}
        case 3:{printf("word error! no = after !\n");break;}
        case 4:{printf("word error! can't recognize the character!\n");break;}
        case 5:{printf("word error! number after zero!\n");break;}

        case 6:{printf("no identity type for declaration\n");break;}
        case 7:{printf("wrong word for identity\n");break;}
        case 8:{printf("no semicolon\n");break;}
        case 9:{printf("no equality sign\n");break;}
        case 10:{printf("wrong type for assign identity\n");break;}
        case 11:{printf("wrong array offset\n");break;}
        case 12:{printf("bracket missed\n");break;}//[]
        case 13:{printf("wrong number\n");break;}
        case 14:{printf("parentheses missed\n");break;}//()
        case 15:{printf("brace missed\n");break;}//{}
        case 16:{printf("function hasn't been declared\n");break;}
        case 17:{printf("wrong condition\n");break;}
        case 18:{printf("no reserve word\n");break;}
        case 19:{printf("no colon\n");break;}
        case 20:{printf("calling wrong func type\n");break;}
        case 21:{printf("calling wrong para number\n");break;}
        case 22:{printf("wrong var\n");break;}
        case 23:{printf("wrong return type for the function\n");break;}
        case 24:{printf("wrong array offset\n");break;}
        case 25:{printf("wrong factor\n");break;}
        case 26:{printf("no main function\n");break;}
        case 27:{printf("wrong declaration position\n");break;}
        case 28:{printf("wrong identity!\n");break;}
        case 29:{printf("couldn't recogonize the sentence!\n");break;}
        case 30:{printf("wrong function type!\n");break;}

        case 31:{printf("identity has been declared in this function!\n");break;}
        case 32:{printf("wrong case type!\n");break;}
        case 33:{printf("wrong para type!\n");break;}
    }
}
