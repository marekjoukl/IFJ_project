#include "../src/generator.h"

int main(void){
    Generator g;
    StartGenerator(&g);
    printf("%s", g.header.str);
    printf("%s", g.instructions.str);
    printf("%s", g.footer.str);
    cleanup_generator(&g);

    return 0;
}
