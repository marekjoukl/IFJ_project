#include "parser.h"
#include <stdio.h>

int main(int argc, char *argv[]){

    FILE *input = stdin;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            printf("Could not open file %s\n", argv[1]);
             return 1;
        }
    }
    Lexeme token;
    bool check = true;
    while (true)
    {
        token = get_next_non_whitespace_lexeme();     
        check = Prog(token);
        if (!check){
            printf("ERROR\n");
            break;
        }
        else if (check) {
            printf("funguje\n");
            break;
        }
    }
    fclose(input);
    return 0;
}
