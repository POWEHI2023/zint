#ifndef __ZMEMORY_CONTROL_IMPL__
#define __ZMEMORY_CONTROL_IMPL__

#include "../tools/memory_control.h"

zint* __zpage_zint_allocate(zpage* __page)
{
    return zint_make_nil();
}

zelem* __zpage_zelem_allocate(zpage* __page)
{
    return zelem_make();
}

zint* __zcache_zint_allocate(void)
{
    return __zpage_zint_allocate(0);
}

zelem* __zcache_zelem_allocate(void)
{
    return __zpage_zelem_allocate(0);
}

zint* zint_allocate(void)
{
    return __zcache_zint_allocate();
}

zelem* zelem_allocate()
{
    return __zcache_zelem_allocate();
}

int zelem_allocate_free(zelem* __elem)
{
    zelem_free(__elem);

    return 0;
}

int zint_allocate_free(zint* __x)
{
    zint_free(__x);

    return 0;
}

#endif