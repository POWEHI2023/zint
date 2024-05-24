#ifndef __ZBSTRUCT__
#define __ZBSTRUCT__

#include "link_list.h"
#include "memory_control.h"

typedef __INT64_TYPE__ pointer;

typedef struct zelem
{
    __INT32_TYPE__  val;
    link_list       list;
} zelem;

/**
 * sign intro: 
 * 
 * 1    2   3   4   5   6   7   8
 * -    -   -   -   -   -   -   pos/neg
*/
typedef struct zint
{
    __INT8_TYPE__   sign;
    zelem           head;

    __INT32_TYPE__  capacity;
    __INT32_TYPE__  used;
} zint;

#define __PNSIGN 0x01

#define is_negative(x) \
({  \
    typeid(x) == typeid(zint);      \
    (bool)(x->sign & __PNSIGN);     \
});

#define is_positive(x) (is_negative(x) ^ 0x01)









#define zelem_next(e) \
({  \
    link_list* _next = link_lisk_next(&((e)->list)); \
    zelem* __ret = 0;   \
    if (_next != 0) \
        __ret = content_of(_next, zelem, list); \
    __ret;  \
})

#define zelem_front(e) \
({  \
    link_list* _front = link_list_front(&((e)->list));   \
    zelem* __ret = 0;   \
    if (_front != 0)    \
        __ret = content_of(_front, zelem, list);    \
    __ret;  \
})

#define zelem_insert(e, _elem) \
do {    \
    if (e == 0) \
    {           \
        e = _elem;  \
        break;      \
    }               \
    link_list_insert(&(e->list), &(_elem->list)); \
} while (0)

#define zelem_remove(e) \
do {    \
    if (e == 0) break;          \
    link_list_remove(&(e->list));  \
} while (0)

#define zelem_reset(_elem) \
do {    \
    zelem* _e = (zelem*)_elem;  \
    _e->val = 0;                \
    link_list_reset(&(_e->list));  \
} while (0);

#define zelem_make() \
({  \
    void* _mem = malloc(sizeof(zelem)); \
    zelem_reset(_mem);          \
    (zelem*)_mem;       \
})

#define zelem_free(_elem) (free(_elem))

#define SIZEOF_ZELEM (sizeof(zelem))



// initial count of zelems in a zint
#define DEFAULT_ZINT_SPACE 1

#define zint_make_nil() \
({  \
    void* _mem = malloc(sizeof(zint));  \
    ((zint*)_mem)->sign = 0; \
    ((zint*)_mem)->capacity = DEFAULT_ZINT_SPACE;       \
    ((zint*)_mem)->used = 1;                            \
    \
    zelem_reset(&(((zint*)_mem)->head));    \
    (zint*)_mem;    \
})

#define zint_free(_mem) \
do {    \
    zelem* __link = &(((zint*)_mem)->head); \
    __link = zelem_next(__link);    \
    while (__link != 0) \
    {   \
        zelem* __next = zelem_next(__link); \
        zelem_free(__link); \
        __link = __next;    \
    }   \
} while (0)

// 
#define ZELEM_LIMIT_NUM 1000000000

zint* zint_make_ll(long long _num);
zint* zint_make(char const* _num);

void zint_add(zint* x, zint y);
void zint_sub(zint* x, zint y);
void zint_mul(zint* x, zint y);
void zint_div(zint* x, zint y);

char const* zint_to_str(zint* x);

#endif


// 0461168601413242100
// 9223372036854775807