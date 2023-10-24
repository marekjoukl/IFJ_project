#include "scanner.h"
#include "automat.h"
#include <stdio.h>

int main(void)
{
    Lexeme lexeme;
    lexeme = get_lexeme();
    printf("%d\n", lexeme.kind);

    if (lexeme.kind == IDENTIFIER || lexeme.kind == STRING || lexeme.kind == MULTILINE_STRING)
    {
        printf("%s\n", lexeme.extra_data.string);
    }
    
    return 0;
}
