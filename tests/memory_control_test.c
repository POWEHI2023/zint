
#include "../tools_impl/memory_control.c"

int private_memory_test(void)
{
    void* _mem = make_private_memory(1024);
    printf("Returned ptr: %ld\n", (size_t)_mem);

    void* _mem1 = _mem - __BACK_OFFSET;
    __BACK_TYPE x = *(__BACK_TYPE*)(_mem1);

    printf("Total size[back trace]: %ld\n\n", x);

    unmap_private_memory(_mem);

    return 0;
}

int file_memory_test(void)
{
    return 0;
}

int zcache_test(void)
{
    zcache_init();
    zcache_display();

    zcache_free();

    return 0;
}

int zcache_allocator_test(void)
{
    zint* x = zint_allocate();

    zint_display(x);

    zint_allocate_free(x);

    return 0;
}

int memory_control_test(void)
{
    printf("\n== memory_control_test ==\n");

    private_memory_test();
    file_memory_test();

    zcache_test();
    zcache_allocator_test();

    return 0;
}