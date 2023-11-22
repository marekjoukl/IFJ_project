#include "parser.h"

int main(void){
    Lexeme token;
    bool check = true;
    while (true)
    {
        token = get_lexeme();
        check = Prog(token);
        if (!check){
            fprintf(stdout, "ERROR");
            break;
        }
    }
    return 0;
}
