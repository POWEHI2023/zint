#ifndef __ZBSTRUCT_IMPL__
#define __ZBSTRUCT_IMPL__

#include "../tools/basic_struct.h"

zint* zint_make_ll(long long _num)
{
    return 0;
}

zint* zint_make(char const* _num)
{
    return 0;
}

void zint_add_0(zint* x, __INT32_TYPE__ y)
{
    // printf("zint add 1\n");

    zelem* __crt = &(x->head);
    __INT32_TYPE__ __remain = ZELEM_LIMIT_NUM - __crt->val;
    __INT64_TYPE__ __buffer = 0;

    while (__remain <= y)
    {
        // printf("in while\n");

        __buffer = (__INT64_TYPE__)(__crt->val) + (__INT64_TYPE__)(y);
        __crt->val = __buffer % ZELEM_LIMIT_NUM;
        y = __buffer / ZELEM_LIMIT_NUM;

        if (__crt->list.next == 0)
        {
            zelem* __elem = zelem_allocate();
            zelem_insert(__crt, __elem);
        }
        __crt = zelem_next(__crt);
        __remain = ZELEM_LIMIT_NUM - __crt->val;
    }

    // printf("out while: __crt_val=%ld, add y=%ld\n", (size_t)(__crt->val), (size_t)(y));
    __crt->val += y;
    // printf("result: %ld\n", (size_t)(__crt->val));
}

void zint_add(zint* x, zint y)
{
    
}

void zint_sub(zint* x, zint y)
{

}

void zint_mul(zint* x, zint y)
{

}

void zint_div(zint* x, zint y)
{

}

// debug, just print out
void zelem_to_str(zelem* e, __INT8_TYPE__ __tag)
{
    char buffer[9] = {'0'};
    int __size = 9;
}

char const* zint_to_str(zint* x)
{
    __INT8_TYPE__ __tag = 0;

}

#endif