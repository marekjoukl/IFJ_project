#include "parser.h" 
#include <stdio.h>

int main(void){
    StartParser();
    fprintf(stdout, ".IFJcode23\n");
    fprintf(stdout, "WRITE string@Hello\\032world!\\010");
    return 0;
}
