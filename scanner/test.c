#include "scanner.h"
#include "automat.h"
#include <stdio.h>

int main(void)
{
    Lexeme lexeme;
    lexeme = get_lexeme();
    printf("%d\n", lexeme.kind);
    free(lexeme.extra_data.string);
    return 0;
}
