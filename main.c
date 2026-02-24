#include "dynamic-string.h"
#include <stdio.h>

int main(void) {
    DString dstr = dstr_create();

    // 示例使用
    if (dstr_assign_cstr(dstr, "Hello") != DSTRING_SUCCESS) {
        return 1;
    }

    if (dstr_append_cstr(dstr, " World!") != DSTRING_SUCCESS) {
        return 1;
    }

    printf("String: %s\n", dstr_cstr(dstr));
    printf("Length: %zu\n", dstr_length(dstr));
    printf("Capacity: %zu\n", dstr_capacity(dstr));

    dstr_destroy(dstr);
    return 0;
}