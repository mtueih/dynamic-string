#include <stdio.h>
#include "dynamic_string.h"

int main(void) {
    dstr_t *str = dstr_create("Hello World!");

    printf("%s\n", dstr_cstr(str));

    dstr_replace_cstr(str, "l", "LLL", 0, false);

    printf("%s\n", dstr_cstr(str));


    dstr_destroy(str);
    return 0;
}