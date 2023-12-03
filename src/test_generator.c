#include "generator.h"

int main(void){
    Generator g;
    StartGenerator(&g);
    print_header(&g);
    print_footer(&g);
    printf("%s", g.header.str);
    printf("%s", g.instructions.str);
    printf("%s", g.footer.str);
    cleanup(&g);

    return 0;
}
