
#include "../tools_impl/memory_control.c"
#include "../tools_impl/basic_struct.c"

int basic_struct_test(void)
{
    printf("\n== basic_struct_test ==\n");

    zint* x = zint_allocate();

    zint_add_0(x, 1);
    printf("head_val[%ld]\n", x->head.val);

    zint_display(x);

    zint_add_0(x, ZELEM_LIMIT_NUM);
    zint_display(x);

    for (int i = 0; i < ZELEM_LIMIT_NUM; ++i)
        zint_add_0(x, ZELEM_LIMIT_NUM);

    zint_display(x);

    zint_allocate_free(x);
    return 0;
}