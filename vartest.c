#include <stdio.h>
#include <stdarg.h> //变长数组需要


void printstrings(const char *fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stdout,fmt,ap);
    fprintf(stdout,"\n");
    fflush(stdout);
}



int main(){
    printstrings("we have nothing");
    printstrings("we have %s","a horse");
    printstrings("we have %s,%s","a horse","an apple");


    
}