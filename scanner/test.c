#include "scanner.h"
#include "automat.h"
#include <stdio.h>

int main(void)
{
    Lexeme lexeme;
    while (true)
    {
        lexeme = get_lexeme();
        printf("lexeme kind: %d\n", lexeme.kind);
        if (lexeme.kind == LEX_EOF) break;
        if (lexeme.kind == IDENTIFIER || lexeme.kind == STRING || lexeme.kind == MULTILINE_STRING)
        {
            printf("%s\n", lexeme.extra_data.string);
        }
        if (lexeme.kind == INTEGER)
        {
            //printf("%s\n", "dubeg");
            printf("%d\n", lexeme.extra_data.IntValue);
        }
        if (lexeme.kind == DOUBLE)
        {
            printf("%f\n", lexeme.extra_data.DoubleValue);
        }
    }
    
    return 0;
}
